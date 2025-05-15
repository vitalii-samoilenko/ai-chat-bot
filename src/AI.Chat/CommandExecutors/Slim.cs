namespace AI.Chat.CommandExecutors
{
    public class Slim : ICommandExecutor
    {
        private readonly System.Collections.Generic.Dictionary<string, ICommand> _commands;

        public Slim(System.Collections.Generic.IEnumerable<ICommand> commands)
            : this(commands, new System.Collections.Generic.Dictionary<System.Type, string>())
        {

        }
        public Slim(System.Collections.Generic.IEnumerable<ICommand> commands, System.Collections.Generic.IReadOnlyDictionary<System.Type, string> overrides)
        {
            _commands = new System.Collections.Generic.Dictionary<string, ICommand>(System.StringComparer.OrdinalIgnoreCase);
            foreach (var command in commands)
            {
                var type = command.GetType();
                var key = overrides.ContainsKey(type)
                    ? overrides[type]
                    : type.Name;
                _commands.Add(key, command);
            }
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string command, string args)
        {
            if (_commands.TryGetValue(command, out var target))
            {
                foreach (var token in target.Execute(args))
                {
                    yield return token;
                }
            }
        }
    }
}
