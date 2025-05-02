namespace AI.Chat.Users
{
    public class Slim : IUser
    {
        private readonly Options.User _options;
        private readonly IModerator _moderator;
        private readonly IBot _bot;

        public Slim(Options.User options, IModerator moderator, IBot bot)
        {
            _options = options;
            _moderator = moderator;
            _bot = bot;
        }

        public async System.Threading.Tasks.Task WelcomeAsync(string username, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync)
        {
            if (!_moderator.IsAllowed(_options.Name, username)
                || !_moderator.IsWelcomed(_options.Name, username))
            {
                return;
            }

            var greeting = string.Format(_options.Greeting, username);

            (var greetingKey, var replyKey) = await _bot.ReplyAsync(greeting)
                .ConfigureAwait(false);

            if (_bot.TryGet(replyKey, out var reply))
            {
                if (!_moderator.IsAllowed(_options.Name, username)
                    || !_moderator.IsWelcomed(_options.Name, username)
                    || !_moderator.Greet(username))
                {
                    _bot.Remove(greetingKey, replyKey);
                    return;
                }
                if (_moderator.IsModerated(_options.Name, username))
                {
                    _moderator.Hold(replyKey,
                    (
                        async () => await onAllowAsync(reply)
                            .ConfigureAwait(false),
                        () =>
                        {
                            _bot.Remove(greetingKey, replyKey);
                            return System.Threading.Tasks.Task.CompletedTask;
                        }
                    ));
                    await onHoldAsync($"{replyKey}: {reply}")
                        .ConfigureAwait(false);
                    return;
                }

                await onAllowAsync(reply)
                    .ConfigureAwait(false);
            }
        }
        public async System.Threading.Tasks.Task ChatAsync(string username, string message, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync)
        {
            if (!_moderator.IsAllowed(_options.Name, username))
            {
                return;
            }

            var prompt = string.Format(_options.Prompt, username, message);

            (var promptKey, var replyKey) = await _bot.ReplyAsync(prompt)
                .ConfigureAwait(false);

            if (_bot.TryGet(replyKey, out var reply))
            {
                if (!_moderator.IsAllowed(_options.Name, username))
                {
                    _bot.Remove(promptKey, replyKey);
                    return;
                }
                if (_moderator.IsModerated(_options.Name, username))
                {
                    _moderator.Hold(replyKey,
                    (
                        async () => await onAllowAsync(reply)
                            .ConfigureAwait(false),
                        () =>
                        {
                            _bot.Remove(promptKey, replyKey);
                            return System.Threading.Tasks.Task.CompletedTask;
                        }
                    ));
                    await onHoldAsync($"{replyKey}: {reply}")
                        .ConfigureAwait(false);
                    return;
                }

                await onAllowAsync(reply)
                    .ConfigureAwait(false);
            }
        }
    }
}
