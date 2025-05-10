namespace AI.Chat.Adapters.Diagnostics
{
    public class Tokens<TAdapter> : IAdapter
        where TAdapter : IAdapter
    {
        private readonly IAdapter _adapter;

        public Tokens(TAdapter adapter)
        {
            _adapter = adapter;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            (var reply, var tokens) = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            AI.Chat.Diagnostics.Meters.Tokens.Record(tokens);
            return (reply, tokens);
        }
    }
}
