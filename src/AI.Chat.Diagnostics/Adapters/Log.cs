using Microsoft.Extensions.Logging;

namespace AI.Chat.Adapters.Diagnostics
{
    public class Log<TAdapter> : IAdapter
        where TAdapter : IAdapter
    {
        private readonly IAdapter _adapter;
        private readonly ILogger<Log<TAdapter>> _logger;

        public Log(TAdapter adapter, ILogger<Log<TAdapter>> logger)
        {
            _adapter = adapter;
            _logger = logger;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            (var reply, var tokens) = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            _logger.LogInformation(reply);
            return (reply, tokens);
        }
    }
}
