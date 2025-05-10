namespace AI.Chat.Histories
{
    public class ThreadSafe<THistory> : IHistory
        where THistory : IHistory
    {
        private readonly IHistory _history;
        private readonly IScope _scope;

        public ThreadSafe(THistory history, IScope scope)
        {
            _history = history;
            _scope = scope;
        }

        public System.DateTime Add(Record record)
        {
            return _scope.ExecuteWrite(() => _history.Add(record));
        }
        public void Remove(params System.DateTime[] keys)
        {
            _scope.ExecuteWrite(() => _history.Remove(keys));
        }
        public void Clear()
        {
            _scope.ExecuteWrite(() => _history.Clear());
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, params string[] tags)
        {
            foreach (var key in _scope.ExecuteRead(() => _history.Find(fromKey, toKey, tags)))
            {
                yield return key;
            }
        }
        public bool TryGet(System.DateTime key, out Record record)
        {
            Record temp = default;
            var result = _scope.ExecuteRead(() => _history.TryGet(key, out temp));
            record = temp;
            return result;
        }
    }
}
