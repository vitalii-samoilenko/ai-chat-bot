using AI.Chat.Extensions;

namespace AI.Chat.Adapters
{
    public class Limited<TAdapter> : IAdapter
        where TAdapter : IAdapter
    {
        private readonly Options.Adapter _options;
        private readonly IAdapter _adapter;
        private readonly IHistory _history;

        public Limited(Options.Adapter options, TAdapter adapter, IHistory history)
        {
            _options = options;
            _adapter = adapter;
            _history = history;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            (var reply, var tokens) = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            if (_options.Threshold < tokens)
            {
                var fromKey = System.DateTime.MinValue;
                var toKey = System.DateTime.MaxValue;
                var skip = 0;
                foreach (var key in _history.Find(fromKey, toKey))
                {
                    fromKey = key;
                    if (_options.Skip < ++skip)
                    {
                        break;
                    }
                }
                toKey = fromKey + _options.Period;
                var keys = new System.Collections.Generic.List<System.DateTime>(_history.Find(fromKey, toKey));
                _history.Remove(keys);
            }
            return (reply, tokens);
        }
    }
}
