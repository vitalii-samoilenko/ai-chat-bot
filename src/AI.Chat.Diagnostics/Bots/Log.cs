using Microsoft.Extensions.Logging;

namespace AI.Chat.Bots.Diagnostics
{
    public class Log<TBot> : IBot
        where TBot : IBot
    {
        private readonly IBot _bot;
        private readonly ILogger<Log<TBot>> _logger;

        public Log(TBot bot, ILogger<Log<TBot>> logger)
        {
            _bot = bot;
            _logger = logger;
        }

        public async System.Threading.Tasks.Task<(System.DateTime messageKey, System.DateTime replyKey)> ReplyAsync(string username, string message)
        {
            _logger.LogInformation(message);
            return await _bot.ReplyAsync(username, message)
                .ConfigureAwait(false);
        }
    }
}
