namespace AI.Chat.Bots
{
    public class Filtered : IBot
    {
        private readonly Options.Bot _options;
        private readonly IAdapter _adapter;
        private readonly System.Collections.Generic.IEnumerable<IFilter> _filters;

        public Filtered(Options.Bot options, IAdapter adapter, System.Collections.Generic.IEnumerable<IFilter> filters)
        {
            _options = options;
            _adapter = adapter;
            _filters = filters;
        }

        public bool TryGet(string key, out string message)
        {
            return _adapter.TryGet(key, out message);
        }
        public void Remove(params string[] keys)
        {
            foreach (var key in keys)
            {
                _adapter.Remove(key);
            }
        }
        public void RemoveAll()
        {
            _adapter.RemoveAll();
        }
        public string Instruct(string message)
        {
            return _adapter.AddInstruction(message);
        }
        public System.Collections.Generic.IEnumerable<string> Find(string fromKey, string toKey)
        {
            return _adapter.Find(fromKey, toKey);
        }
        public System.Collections.Generic.IEnumerable<string> FindAll()
        {
            return _adapter.FindAll();
        }

        public async System.Threading.Tasks.Task<(string messageKey, string replyKey)> ReplyAsync(string message)
        {
            var filterKeys = new System.Collections.Generic.Dictionary<IFilter, string>();
            var warnings = 0;
            string reply = null;
            var messageKey = _adapter.AddMessage(message);
            for (var denied = true; denied;)
            {
                denied = false;
                reply = await _adapter.GetReplyAsync()
                    .ConfigureAwait(false);
                foreach (var filter in _filters)
                {
                    if (!filter.IsDenied(reply))
                    {
                        continue;
                    }
                    if (_options.Warnings < ++warnings)
                    {
                        reply = _options.Apology;
                    }
                    else
                    {
                        denied = true;
                        if (!filterKeys.ContainsKey(filter))
                        {
                            var promptKey = _adapter.AddInstruction(filter.Prompt);
                            filterKeys.Add(filter, promptKey);
                        }
                    }
                    break;
                }
            }
            foreach (var promptKey in filterKeys.Values)
            {
                _adapter.Remove(promptKey);
            }
            var replyKey = _adapter.AddReply(reply);
            return (messageKey, replyKey);
        }
    }
}
