namespace AI.Chat.Histories.Console
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
            Host.Console.Helpers.AppendLog(key, record);
            return key;
        }
        public void Remove(params System.DateTime[] keys)
        {
            _history.Remove(keys);
            Host.Console.Helpers.DeleteLog(keys);
        }
        public void Clear()
        {
            _history.Clear();
            Host.Console.Helpers.DeleteLog();
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, params string[] tags)
        {
            return _history.Find(fromKey, toKey, tags);
        }
        public bool TryGet(System.DateTime key, out Record record)
        {
            return _history.TryGet(key, out record);
        }
    }
}
