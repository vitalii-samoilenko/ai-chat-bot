using AI.Chat.Extensions;

namespace AI.Chat.Bots
{
    public class Filtered : IBot
    {
        private readonly Options.Bot _options;
        private readonly IAdapter _adapter;
        private readonly IFilter _filter;
        private readonly IHistory _history;

        public Filtered(Options.Bot options, IAdapter adapter, IFilter filter, IHistory history)
        {
            _options = options;
            _adapter = adapter;
            _filter = filter;
            _history = history;
        }

        public async System.Threading.Tasks.Task<(System.DateTime messageKey, System.DateTime replyKey)> ReplyAsync(string username, string message)
        {
            var reasonKeys = new System.Collections.Generic.Dictionary<string, System.DateTime>();
            var warnings = 0;
            string reply = null;
            var messageKey = _history.AddUserMessage(username, message);
            for (var denied = true; denied;)
            {
                denied = false;
                (reply, _) = await _adapter.GetReplyAsync()
                    .ConfigureAwait(false);
                if (!_filter.IsDenied(reply, out var reason))
                {
                    continue;
                }
                if (_options.Warnings < ++warnings)
                {
                    reply = string.Format(_options.Apology, username);
                }
                else
                {
                    denied = true;
                    if (!reasonKeys.ContainsKey(reason))
                    {
                        var reasonKey = _history.AddSystemMessage(reason);
                        reasonKeys.Add(reason, reasonKey);
                    }
                }
            }
            if (0 < reasonKeys.Count)
            {
                _history.Remove(reasonKeys.Values);
            }
            var replyKey = _history.AddModelMessage(reply);
            return (messageKey, replyKey);
        }
    }
}
