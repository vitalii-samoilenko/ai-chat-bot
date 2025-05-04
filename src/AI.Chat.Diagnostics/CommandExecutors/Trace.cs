namespace AI.Chat.CommandExecutors.Diagnostics
{
    public class Trace<TCommandExecutor> : ICommandExecutor
        where TCommandExecutor : ICommandExecutor
    {
        private static string CommandExecutorName = $"{typeof(TCommandExecutor).Namespace}.{typeof(TCommandExecutor).Name}";

        private readonly ICommandExecutor _commandExecutor;

        public Trace(TCommandExecutor commandExecutor)
        {
            _commandExecutor = commandExecutor;
        }

        public async System.Threading.Tasks.Task<bool> ExecuteAsync(string command, string args)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.CommandExecutors.StartActivity($"{CommandExecutorName}.{nameof(ExecuteAsync)}"))
            {
                return await _commandExecutor.ExecuteAsync(command, args)
                    .ConfigureAwait(false);
            }
        }
    }
}
