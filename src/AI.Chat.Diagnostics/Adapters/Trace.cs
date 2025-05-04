namespace AI.Chat.Adapters.Diagnostics
{
    public class Trace<TAdapter> : IAdapter
        where TAdapter : IAdapter
    {
        private static string AdapterName = $"{typeof(TAdapter).Namespace}.{typeof(TAdapter).Name}";

        private readonly IAdapter _adapter;

        public Trace(TAdapter adapter)
        {
            _adapter = adapter;
        }

        public string AddInstruction(string content)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(AddInstruction)}"))
            {
                return _adapter.AddInstruction(content);
            }
        }
        public string AddMessage(string content)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(AddMessage)}"))
            {
                return _adapter.AddMessage(content);
            }
        }
        public string AddReply(string content)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(AddReply)}"))
            {
                return _adapter.AddReply(content);
            }
        }
        public bool TryGet(string key, out string content)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(TryGet)}"))
            {
                return _adapter.TryGet(key, out content);
            }
        }
        public void Remove(string key)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(Remove)}"))
            {
                _adapter.Remove(key);
            }
        }
        public void RemoveAll()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(RemoveAll)}"))
            {
                _adapter.RemoveAll();
            }
        }
        public System.Collections.Generic.IEnumerable<string> Find(string fromKey, string toKey)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(Find)}"))
            {
                return _adapter.Find(fromKey, toKey);
            }
        }
        public System.Collections.Generic.IEnumerable<string> FindAll()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(FindAll)}"))
            {
                return _adapter.FindAll();
            }
        }

        public async System.Threading.Tasks.Task<string> GetReplyAsync()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(GetReplyAsync)}"))
            {
                return await _adapter.GetReplyAsync()
                    .ConfigureAwait(false);
            }
        }
    }
}
