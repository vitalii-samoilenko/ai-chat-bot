using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Remove : ICommand
    {
        private readonly IHistory _history;

        public Remove(IHistory history)
        {
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            if (Constants.ArgsAll.Equals(args, System.StringComparison.OrdinalIgnoreCase))
            {
                _history.Clear();
                yield return args;
            }
            else
            {
                var keys = new System.Collections.Generic.List<System.DateTime>();
                foreach (var arg in args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries))
                {
                    if (!arg.TryParseKey(out var key))
                    {
                        continue;
                    }
                    keys.Add(key);
                }
                foreach (var key in _history.Remove(keys.ToArray()))
                {
                    yield return key.ToKeyString();
                }
            }
        }
    }
}
