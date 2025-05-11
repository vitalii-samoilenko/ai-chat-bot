namespace AI.Chat.Histories.Diagnostics
{
    public class Trace<THistory> : IHistory
        where THistory : IHistory
    {
        private static string HistoryName = $"{typeof(THistory).Namespace}.{typeof(THistory).Name}";

        private readonly IHistory _history;

        public Trace(THistory history)
        {
            _history = history;
        }

        public System.DateTime Add(Record record)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Add)}"))
            {
                return _history.Add(record);
            }
        }
        public void Remove(params System.DateTime[] keys)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Remove)}"))
            {
                _history.Remove(keys);
            }
        }
        public void Clear()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Clear)}"))
            {
                _history.Clear();
            }
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, params string[] tags)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Find)}"))
            {
                foreach (var key in _history.Find(fromKey, toKey, tags))
                {
                    yield return key;
                }
            }
        }
        public bool TryGet(System.DateTime key, out Record record)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(TryGet)}"))
            {
                return _history.TryGet(key, out record);
            }
        }
    }
}
