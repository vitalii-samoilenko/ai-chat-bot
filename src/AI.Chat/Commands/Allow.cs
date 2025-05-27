using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Allow : ICommand
    {
        private readonly IHistory _history;

        public Allow(IHistory history)
        {
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            System.Collections.Generic.List<System.DateTime> keys = null;
            if (Defaults.ArgsAll.Equals(args, System.StringComparison.OrdinalIgnoreCase))
            {
                keys = new System.Collections.Generic.List<System.DateTime>(
                    _history.FindModerated(System.DateTime.MinValue, System.DateTime.MaxValue));
            }
            else
            {
                keys = new System.Collections.Generic.List<System.DateTime>();
                foreach (var arg in args.SplitArgs())
                {
                    if (!arg.TryParseKey(out var key))
                    {
                        continue;
                    }
                    keys.Add(key);
                }
            };
            if (0 < keys.Count)
            {
                foreach (var key in _history.Unmoderate(keys))
                {
                    yield return key.ToKeyString();
                }
            }
        }
    }
}
