using Microsoft.Extensions.Logging;

namespace AI.Chat.CommandExecutors.Diagnostics
{
    public class Log<TCommandExecutor> : ICommandExecutor
        where TCommandExecutor : ICommandExecutor
    {
        private readonly ICommandExecutor _commandExecutor;
        private readonly ILogger<Log<TCommandExecutor>> _logger;

        public Log(TCommandExecutor commandExecutor, ILogger<Log<TCommandExecutor>> logger)
        {
            _commandExecutor = commandExecutor;
            _logger = logger;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string username, string command, string args)
        {
            var count = 0;
            System.Collections.Generic.IEnumerator<string> enumerator = null;
            try
            {
                enumerator = _commandExecutor.Execute(username, command, args).GetEnumerator();
                while (enumerator.MoveNext())
                {
                    ++count;
                    yield return enumerator.Current;
                }
            }
            finally
            {
                if (enumerator != null)
                {
                    enumerator.Dispose();
                }
                _logger.LogInformation("{username}: {command} ({args}) = {count}", username, command, args, count);
            }
        }
    }
}
