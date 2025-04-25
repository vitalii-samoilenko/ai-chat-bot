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

        public async System.Threading.Tasks.Task<bool> ExecuteAsync(string command, string args)
        {
            if (_commands.TryGetValue(command, out var target))
            {
                await target.ExecuteAsync(args)
                    .ConfigureAwait(false);
                return true;
            }
            return false;
        }
    }
}
