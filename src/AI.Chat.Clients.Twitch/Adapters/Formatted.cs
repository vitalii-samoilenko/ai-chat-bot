namespace AI.Chat.Adapters.Twitch
{
    public class Formatted<TAdapter> : IAdapter
        where TAdapter : IAdapter
    {
        private readonly IAdapter _adapter;

        public Formatted(TAdapter adapter)
        {
            _adapter = adapter;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            (var reply, var tokens) = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            return (reply.Replace(System.Environment.NewLine, " ").Trim(), tokens);
        }
    }
}
