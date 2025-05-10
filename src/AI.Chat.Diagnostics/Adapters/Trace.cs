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

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Adapters.StartActivity($"{AdapterName}.{nameof(GetReplyAsync)}"))
            {
                return await _adapter.GetReplyAsync()
                    .ConfigureAwait(false);
            }
        }
    }
}
