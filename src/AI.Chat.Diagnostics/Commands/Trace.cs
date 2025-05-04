namespace AI.Chat.Commands.Diagnostics
{
    public class Trace<TCommand> : ICommand
        where TCommand : ICommand
    {
        private static string CommandName = $"{typeof(TCommand).Namespace}.{typeof(TCommand).Name}";

        private readonly ICommand _command;

        public Trace(TCommand command)
        {
            _command = command;
        }

        public async System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Commands.StartActivity($"{CommandName}.{nameof(ExecuteAsync)}"))
            {
                await _command.ExecuteAsync(args)
                    .ConfigureAwait(false);
            }
        }
    }
}
