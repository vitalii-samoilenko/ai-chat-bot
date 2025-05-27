using AI.Chat.Adapters.Extensions.GoogleAI;
using AI.Chat.Extensions;

namespace AI.Chat.Histories.GoogleAI
{
    public class Tracked<THistory> : IHistory
        where THistory : IHistory
    {
        private readonly IHistory _history;
        private readonly System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content> _contents;
        private readonly System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content> _moderated;

        public Tracked(THistory history, System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content> contents)
        {
            _history = history;
            _contents = contents;
            _moderated = new System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content>();
        }

        public System.DateTime Add(Record record)
        {
            var key = _history.Add(record);
            var content = record.ToContent();
            if (record.IsModerated())
            {
                _moderated.Add(key, content);
            }
            else
            {
                _contents.Add(key, content);
            }
            return key;
        }
        public System.Collections.Generic.List<System.DateTime> Remove(System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var removed = _history.Remove(keys);
            foreach (var key in removed)
            {
                if (!_contents.Remove(key))
                {
                    _moderated.Remove(key);
                }
            }
            return removed;
        }
        public void Clear()
        {
            _history.Clear();
            _contents.Clear();
            _moderated.Clear();
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
            if (result)
            {
                if (!_contents.TryGet(key, out var entry))
                {
                    entry = _moderated[key];
                }
                entry.Value.Parts[0].Text = message;
            }
            return result;
        }
        public System.Collections.Generic.List<System.DateTime> Tag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var tagged = _history.Tag(tag, keys);
            if (Defaults.TagModerated.Equals(tag, System.StringComparison.OrdinalIgnoreCase))
            {
                foreach (var key in tagged)
                {
                    var content = _contents[key].Value;
                    _contents.Remove(key);
                    _moderated.Add(key, content);
                }
            }
            return tagged;
        }
        public System.Collections.Generic.List<System.DateTime> Untag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var untagged = _history.Untag(tag, keys);
            if (Defaults.TagModerated.Equals(tag, System.StringComparison.OrdinalIgnoreCase))
            {
                foreach (var key in untagged)
                {
                    var content = _moderated[key].Value;
                    _moderated.Remove(key);
                    _contents.Add(key, content);
                }
            }
            return untagged;
        }
    }
}
