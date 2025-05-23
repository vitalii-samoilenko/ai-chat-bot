namespace AI.Chat.Commands
{
    public class ThreadSafeRead<TCommand> : ICommand
        where TCommand : ICommand
    {
        private readonly ICommand _command;
        private readonly IScope _scope;

        public ThreadSafeRead(TCommand command, IScope scope)
        {
            _command = command;
            _scope = scope;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            foreach (var token in _scope.ExecuteRead(() => _command.Execute(args)))
            {
                yield return token;
            }
        }
    }
}
