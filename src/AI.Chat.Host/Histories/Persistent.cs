namespace AI.Chat.Histories.Host
{
    public class Persistent<THistory> : IHistory
        where THistory : IHistory
    {
        private readonly IHistory _history;

        public Persistent(THistory history)
        {
            _history = history;
        }

        public System.DateTime Add(Record record)
        {
            var key = _history.Add(record);
            AI.Chat.Host.Helpers.AppendLog(key, record);
            return key;
        }
        public System.DateTime[] Remove(params System.DateTime[] keys)
        {
            var removed = _history.Remove(keys);
            if (0 < removed.Length)
            {
                AI.Chat.Host.Helpers.DeleteLog(removed);
            }
            return removed;
        }
        public void Clear()
        {
            _history.Clear();
            AI.Chat.Host.Helpers.DeleteLog();
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
                AI.Chat.Host.Helpers.EditLog(key, message);
            }
            return result;
        }
        public System.DateTime[] Tag(string tag, params System.DateTime[] keys)
        {
            var tagged = _history.Tag(tag, keys);
            if (0 < tagged.Length)
            {
                AI.Chat.Host.Helpers.TagLog(tag, tagged);
            }
            return tagged;
        }
        public System.DateTime[] Untag(string tag, params System.DateTime[] keys)
        {
            var tagged = _history.Untag(tag, keys);
            if (0 < tagged.Length)
            {
                AI.Chat.Host.Helpers.UntagLog(tag, tagged);
            }
            return tagged;
        }
    }
}
