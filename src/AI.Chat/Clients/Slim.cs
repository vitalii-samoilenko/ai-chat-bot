using AI.Chat.Extensions;

namespace AI.Chat.Clients
{
    public class Slim : IClient
    {
        private readonly Options.Client _options;
        private readonly IModerator _moderator;
        private readonly IBot _bot;
        private readonly IHistory _history;

        public Slim(Options.Client options, IModerator moderator, IBot bot, IHistory history)
        {
            _options = options;
            _moderator = moderator;
            _bot = bot;
            _history = history;
        }

        public async System.Threading.Tasks.Task WelcomeAsync(string username, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync)
        {
            if (!_moderator.IsAllowed(_options.Username, username)
                || !_moderator.IsWelcomed(_options.Username, username))
            {
                return;
            }

            var joined = string.Format(_options.Joined, username);

            (var joinedKey, var replyKey) = await _bot.ReplyAsync(username, joined)
                .ConfigureAwait(false);

            if (!_history.TryGet(replyKey, out var reply))
            {
                return;
            }
            if (!_moderator.IsAllowed(_options.Username, username)
                || !_moderator.IsWelcomed(_options.Username, username)
                || (username != _options.Username
                    && !_moderator.Greet(username)))
            {
                _history.Remove(joinedKey, replyKey);
                return;
            }
            if (_moderator.IsModerated(_options.Username, username))
            {
                _moderator.Hold(replyKey,
                (
                    async () => await onAllowAsync(reply.Message)
                        .ConfigureAwait(false),
                    () =>
                    {
                        _history.Remove(joinedKey, replyKey);
                        return System.Threading.Tasks.Task.CompletedTask;
                    }
                ));
                await onHoldAsync($"{replyKey.ToKeyString()}: {reply}")
                    .ConfigureAwait(false);
                return;
            }

            await onAllowAsync(reply.Message)
                .ConfigureAwait(false);
        }
        public async System.Threading.Tasks.Task ChatAsync(string username, string message, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync)
        {
            if (!_moderator.IsAllowed(_options.Username, username))
            {
                return;
            }

            var prompt = string.Format(_options.Prompt, username, message);

            (var promptKey, var replyKey) = await _bot.ReplyAsync(username, prompt)
                .ConfigureAwait(false);

            if (!_history.TryGet(replyKey, out var reply))
            {
                return;
            }
            if (!_moderator.IsAllowed(_options.Username, username))
            {
                _history.Remove(promptKey, replyKey);
                return;
            }
            if (_moderator.IsWelcomed(_options.Username, username))
            {
                _moderator.Greet(username);
            }
            if (_moderator.IsModerated(_options.Username, username))
            {
                _moderator.Hold(replyKey,
                (
                    async () => await onAllowAsync(reply.Message)
                        .ConfigureAwait(false),
                    () =>
                    {
                        _history.Remove(promptKey, replyKey);
                        return System.Threading.Tasks.Task.CompletedTask;
                    }
                ));
                await onHoldAsync($"{replyKey.ToKeyString()}: {reply}")
                    .ConfigureAwait(false);
                return;
            }

            await onAllowAsync(reply.Message)
                .ConfigureAwait(false);
        }
    }
}
