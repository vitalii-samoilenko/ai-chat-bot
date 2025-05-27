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
        public System.Collections.Generic.List<System.DateTime> Remove(System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Remove)}"))
            {
                return _history.Remove(keys);
            }
        }
        public void Clear()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Clear)}"))
            {
                _history.Clear();
            }
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, System.Collections.Generic.IEnumerable<string> tags)
        {
            var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Find)}");
            System.Collections.Generic.IEnumerator<System.DateTime> enumerator = null;
            try
            {
                enumerator = _history.Find(fromKey, toKey, tags).GetEnumerator();
                while (enumerator.MoveNext())
                {
                    yield return enumerator.Current;
                }
            }
            finally
            {
                if (activity != null)
                {
                    activity.Dispose();
                }
                if (enumerator != null)
                {
                    enumerator.Dispose();
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
        public bool TryEdit(System.DateTime key, string message)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(TryEdit)}"))
            {
                return _history.TryEdit(key, message);
            }
        }
        public System.Collections.Generic.List<System.DateTime> Tag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Tag)}"))
            {
                return _history.Tag(tag, keys);
            }
        }
        public System.Collections.Generic.List<System.DateTime> Untag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Histories.StartActivity($"{HistoryName}.{nameof(Untag)}"))
            {
                return _history.Untag(tag, keys);
            }
        }
    }
}
