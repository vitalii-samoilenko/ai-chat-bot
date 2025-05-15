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

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Commands.StartActivity($"{CommandName}.{nameof(Execute)}"))
            {
                foreach (var token in _command.Execute(args))
                {
                    yield return token;
                }
            }
        }
    }
}
