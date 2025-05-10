namespace AI.Chat.Adapters
{
    public class Delayed<TAdapter> : IAdapter
        where TAdapter : IAdapter
    {
        private readonly Options.Adapter _options;
        private readonly IAdapter _adapter;

        public Delayed(Options.Adapter options, TAdapter adapter)
        {
            _options = options;
            _adapter = adapter;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            (var reply, var tokens) = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            await System.Threading.Tasks.Task.Delay(_options.Delay)
                .ConfigureAwait(false);
            return (reply, tokens);
        }
    }
}
