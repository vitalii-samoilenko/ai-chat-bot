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
        public void Remove(params System.DateTime[] keys)
        {
            _history.Remove(keys);
            AI.Chat.Host.Helpers.DeleteLog(keys);
        }
        public void Clear()
        {
            _history.Clear();
            AI.Chat.Host.Helpers.DeleteLog();
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
                AI.Chat.Host.Helpers.EditLog(key, message);
            }
            return result;
        }
    }
}
