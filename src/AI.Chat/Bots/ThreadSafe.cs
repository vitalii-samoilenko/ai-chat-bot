namespace AI.Chat.Bots
{
    public class ThreadSafe<TBot> : IBot
        where TBot : IBot
    {
        private readonly IBot _bot;
        private readonly IScope _scope;

        public ThreadSafe(TBot bot, IScope scope)
        {
            _bot = bot;
            _scope = scope;
        }

        public async System.Threading.Tasks.Task<(System.DateTime messageKey, System.DateTime replyKey)> ReplyAsync(string username, string message)
        {
            return await _scope.ExecuteWriteAsync(async () =>
                    await _bot.ReplyAsync(username, message)
                        .ConfigureAwait(false))
                .ConfigureAwait(false);
        }
    }
}
