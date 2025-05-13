using AI.Chat.Adapters.Extensions.OpenAI;

namespace AI.Chat.Histories.OpenAI
{
    public class Tracked<THistory> : IHistory
        where THistory : IHistory
    {
        private readonly IHistory _history;
        private readonly System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage> _messages;

        public Tracked(THistory history, System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage> messages)
        {
            _history = history;
            _messages = messages;
        }

        public System.DateTime Add(Record record)
        {
            var key = _history.Add(record);
            _messages.Add(key, record.ToChatMessage());
            return key;
        }
        public void Remove(params System.DateTime[] keys)
        {
            _history.Remove(keys);
            foreach (var key in keys)
            {
                _messages.Remove(key);
            }
        }
        public void Clear()
        {
            _history.Clear();
            _messages.Clear();
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, params string[] tags)
        {
            return _history.Find(fromKey, toKey, tags);
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
                _messages[key].Value.Content[0] = global::OpenAI.Chat.ChatMessageContentPart.CreateTextPart(message);
            }
            return result;
        }
    }
}
