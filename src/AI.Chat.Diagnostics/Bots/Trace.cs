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

        public bool TryGet(string key, out string message)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Bots.StartActivity($"{BotName}.{nameof(TryGet)}"))
            {
                return _bot.TryGet(key, out message);
            }
        }
        public void Remove(params string[] keys)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Bots.StartActivity($"{BotName}.{nameof(Remove)}"))
            {
                _bot.Remove(keys);
            }
        }
        public void RemoveAll()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Bots.StartActivity($"{BotName}.{nameof(RemoveAll)}"))
            {
                _bot.RemoveAll();
            }
        }
        public string Instruct(string message)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Bots.StartActivity($"{BotName}.{nameof(Instruct)}"))
            {
                return _bot.Instruct(message);
            }
        }
        public System.Collections.Generic.IEnumerable<string> Find(string fromKey, string toKey)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Bots.StartActivity($"{BotName}.{nameof(Find)}"))
            {
                return _bot.Find(fromKey, toKey);
            }
        }
        public System.Collections.Generic.IEnumerable<string> FindAll()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Bots.StartActivity($"{BotName}.{nameof(FindAll)}"))
            {
                return _bot.FindAll();
            }
        }

        public async System.Threading.Tasks.Task<(string messageKey, string replyKey)> ReplyAsync(string message)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Bots.StartActivity($"{BotName}.{nameof(ReplyAsync)}"))
            {
                return await _bot.ReplyAsync(message)
                    .ConfigureAwait(false);
            }
        }
    }
}
