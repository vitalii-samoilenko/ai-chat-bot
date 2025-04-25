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

        public bool TryGet(string key, out string message)
        {
            string temp = null;
            var result = _scope.ExecuteRead(() => _bot.TryGet(key, out temp));
            message = temp;
            return result;
        }
        public void Remove(params string[] keys)
        {
            _scope.ExecuteWrite(() => _bot.Remove(keys));
        }
        public void RemoveAll()
        {
            _scope.ExecuteWrite(() => _bot.RemoveAll());
        }
        public string Instruct(string message)
        {
            return _scope.ExecuteWrite(() => _bot.Instruct(message));
        }
        public System.Collections.Generic.IEnumerable<string> Find(string fromKey, string toKey)
        {
            return _scope.ExecuteRead(() => _bot.Find(fromKey, toKey));
        }
        public System.Collections.Generic.IEnumerable<string> FindAll()
        {
            return _scope.ExecuteRead(() => _bot.FindAll());
        }

        public async System.Threading.Tasks.Task<(string messageKey, string replyKey)> ReplyAsync(string message)
        {
            return await _scope.ExecuteWriteAsync(
                    async () => await _bot.ReplyAsync(message)
                        .ConfigureAwait(false))
                .ConfigureAwait(false);
        }
    }
}
