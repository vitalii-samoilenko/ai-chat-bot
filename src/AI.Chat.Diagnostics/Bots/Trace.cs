namespace AI.Chat.Bots.Diagnostics
{
    public class Trace<TBot> : IBot
        where TBot : IBot
    {
        private static string BotName = $"{typeof(TBot).Namespace}.{typeof(TBot).Name}";

        private readonly IBot _bot;

        public Trace(TBot bot)
        {
            _bot = bot;
        }

        public async System.Threading.Tasks.Task<(System.DateTime messageKey, System.DateTime replyKey)> ReplyAsync(string username, string message)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Bots.StartActivity($"{BotName}.{nameof(ReplyAsync)}"))
            {
                return await _bot.ReplyAsync(username, message)
                    .ConfigureAwait(false);
            }
        }
    }
}
