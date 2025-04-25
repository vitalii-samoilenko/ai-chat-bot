namespace AI.Chat.Commands
{
    public class ThreadSafe<TCommand> : ICommand
        where TCommand : ICommand
    {
        private readonly ICommand _command;
        private readonly IScope _scope;

        public ThreadSafe(ICommand command, IScope scope)
        {
            _command = command;
            _scope = scope;
        }

        public async System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            await _scope.ExecuteWriteAsync(
                    async () => await _command.ExecuteAsync(args)
                        .ConfigureAwait(false))
                .ConfigureAwait(false);
        }
    }
}
