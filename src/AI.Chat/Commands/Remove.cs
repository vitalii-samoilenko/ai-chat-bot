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
            if (args == Constants.ArgsAll)
            {
                _history.Clear();
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
                _history.Remove(keys.ToArray());
                foreach (var key in keys)
                {
                    yield return key.ToKeyString();
                }
            }
        }
    }
}
