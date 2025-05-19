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
        public System.Collections.Generic.List<System.DateTime> Remove(System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            return _scope.ExecuteWrite(() => _history.Remove(keys));
        }
        public void Clear()
        {
            _scope.ExecuteWrite(() => _history.Clear());
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, System.Collections.Generic.IEnumerable<string> tags)
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
        public bool TryEdit(System.DateTime key, string message)
        {
            return _scope.ExecuteWrite(() => _history.TryEdit(key, message));
        }
        public System.Collections.Generic.List<System.DateTime> Tag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            return _scope.ExecuteWrite(() => _history.Tag(tag, keys));
        }
        public System.Collections.Generic.List<System.DateTime> Untag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            return _scope.ExecuteWrite(() => _history.Untag(tag, keys));
        }
    }
}
