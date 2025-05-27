namespace AI.Chat.CommandExecutors
{
    public class Slim : ICommandExecutor
    {
        private readonly System.Collections.Generic.Dictionary<string, ICommand> _commands;
        private readonly IModerator _moderator;

        public Slim(System.Collections.Generic.IEnumerable<ICommand> commands, IModerator moderator)
            : this(commands, new System.Collections.Generic.Dictionary<System.Type, string>(), moderator)
        {
            
        }
        public Slim(System.Collections.Generic.IEnumerable<ICommand> commands, System.Collections.Generic.IReadOnlyDictionary<System.Type, string> overrides, IModerator moderator)
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
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string username, string command, string args)
        {
            if (_moderator.IsModerator(username)
                && _commands.TryGetValue(command, out var target))
            {
                foreach (var token in target.Execute(args))
                {
                    yield return token;
                }
            }
        }
    }
}
