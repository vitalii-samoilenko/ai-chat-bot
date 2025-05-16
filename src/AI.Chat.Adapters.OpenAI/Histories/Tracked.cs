using AI.Chat.Adapters.Extensions.OpenAI;
using AI.Chat.Extensions;

namespace AI.Chat.Histories.OpenAI
{
    public class Tracked<THistory> : IHistory
        where THistory : IHistory
    {
        private readonly IHistory _history;
        private readonly System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage> _messages;
        private readonly System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage> _moderated;

        public Tracked(THistory history, System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage> messages)
        {
            _history = history;
            _messages = messages;
            _moderated = new System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage>();
        }

        public System.DateTime Add(Record record)
        {
            var key = _history.Add(record);
            var message = record.ToChatMessage();
            if (record.IsModerated())
            {
                _moderated.Add(message);
            }
            else
            {
                _messages.Add(key, message);
            }
            return key;
        }
        public System.DateTime[] Remove(params System.DateTime[] keys)
        {
            var removed = _history.Remove(keys);
            foreach (var key in removed)
            {
                if (!_messages.Remove(key))
                {
                    _moderated.Remove(key);
                }
            }
            return removed;
        }
        public void Clear()
        {
            _history.Clear();
            _messages.Clear();
            _moderated.Clear();
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, params string[] tags)
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
                if (!_messages.TryGet(key, out var entry))
                {
                    entry = _moderated[key];
                }
                entry.Value.Content[0] = global::OpenAI.Chat.ChatMessageContentPart.CreateTextPart(message);
            }
            return result;
        }
        public System.DateTime[] Tag(string tag, params System.DateTime[] keys)
        {
            var tagged = _history.Tag(tag, keys);
            if (Constants.TagModerated.Equals(tag, System.StringComparison.OrdinalIgnoreCase))
            {
                foreach (var key in tagged)
                {
                    var message = _messages[key].Value;
                    _messages.Remove(key);
                    _moderated.Add(key, message);
                }
            }
            return tagged;
        }
        public System.DateTime[] Untag(string tag, params System.DateTime[] keys)
        {
            var untagged = _history.Untag(tag, keys);
            if (Constants.TagModerated.Equals(tag, System.StringComparison.OrdinalIgnoreCase))
            {
                foreach (var key in untagged)
                {
                    var message = _moderated[key].Value;
                    _moderated.Remove(key);
                    _messages.Add(key, message);
                }
            }
            return untagged;
        }
    }
}
