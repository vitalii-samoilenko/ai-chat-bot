using AI.Chat.Extensions;

namespace AI.Chat.Histories.GoogleAI
{
    public class Cached<THistory> : IHistory
        where THistory : IHistory
    {
        private readonly Options.GoogleAI.Adapter _options;
        private readonly IHistory _history;

        public Cached(Options.GoogleAI.Adapter options, THistory history)
        {
            _options = options;
            _history = history;
        }

        public System.DateTime Add(Record record)
        {
            var key = _history.Add(record);
            if (record.IsSystemInstruction())
            {
                ExpireCache();
            }
            return key;
        }
        public System.Collections.Generic.List<System.DateTime> Remove(System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var removed = _history.Remove(keys);
            foreach (var key in removed)
            {
                if (_options.Cache.Key < key)
                {
                    continue;
                }
                ExpireCache();
                break;
            }
            return removed;
        }
        public void Clear()
        {
            _history.Clear();
            ExpireCache();
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, System.Collections.Generic.IEnumerable<string> tags)
        {
            foreach (var key in _history.Find(fromKey, toKey, tags))
            {
                yield return key;
            }
        }
        public bool TryGet(System.DateTime key, out Record record)
        {
            return _history.TryGet(key, out record);
        }
        public bool TryEdit(System.DateTime key, string message)
        {
            var result = _history.TryEdit(key, message);
            if (result
                && !(_options.Cache.Key < key))
            {
                ExpireCache();
            }
            return result;
        }
        public System.Collections.Generic.List<System.DateTime> Tag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var tagged = _history.Tag(tag, keys);
            if (Constants.TagModerated.Equals(tag, System.StringComparison.OrdinalIgnoreCase))
            {
                foreach (var key in tagged)
                {
                    if (_options.Cache.Key < key)
                    {
                        continue;
                    }
                    ExpireCache();
                    break;
                }
            }
            return tagged;
        }
        public System.Collections.Generic.List<System.DateTime> Untag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var untagged = _history.Untag(tag, keys);
            if (Constants.TagModerated.Equals(tag, System.StringComparison.OrdinalIgnoreCase))
            {
                foreach (var key in untagged)
                {
                    if (_options.Cache.Key < key)
                    {
                        continue;
                    }
                    ExpireCache();
                    break;
                }
            }
            return untagged;
        }

        private void ExpireCache()
        {
            _options.Cache.Until = System.DateTime.MinValue;
        }
    }
}
