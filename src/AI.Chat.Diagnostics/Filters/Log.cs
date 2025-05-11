using Microsoft.Extensions.Logging;

namespace AI.Chat.Filters.Diagnostics
{
    public class Log<TFilter> : IFilter
        where TFilter : IFilter
    {
        private readonly TFilter _filter;
        private readonly ILogger<Log<TFilter>> _logger;

        public Log(TFilter filter, ILogger<Log<TFilter>> logger)
        {
            _filter = filter;
            _logger = logger;
        }

        public bool IsDenied(string message, out string reason)
        {
            var result = _filter.IsDenied(message, out reason);
            if (result)
            {
                _logger.LogInformation(reason);
            }
            return result;
        }
    }
}
