using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Find : ICommand
    {
        private readonly IHistory _history;

        public Find(IHistory history)
        {
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var fromKey = System.DateTime.MinValue;
            var toKey = System.DateTime.MaxValue;
            if (args.StartsWith(Defaults.ArgsAll, System.StringComparison.OrdinalIgnoreCase))
            {
                args = args.Substring(Defaults.ArgsAll.Length);
            }
            else if(!args.ExtractToken(out args).TryParseKey(out fromKey)
                || !args.ExtractToken(out args).TryParseKey(out toKey))
            {
                yield break;
            }
            foreach (var key in _history.Find(fromKey, toKey, args.SplitArgs()))
            {
                yield return key.ToKeyString();
            }
        }
    }
}
