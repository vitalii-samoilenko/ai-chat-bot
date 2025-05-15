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
            if (args.StartsWith(Constants.ArgsAll))
            {
                args = args.Substring(Constants.ArgsAll.Length);
            }
            else
            {
                var previous = 0;
                var next = args.IndexOf(' ', previous);
                if (next < 0
                    || !args.Substring(previous, next - previous).TryParseKey(out fromKey))
                {
                    yield break;
                }
                previous = next + 1;
                next = args.IndexOf(' ', previous);
                if (next < 0)
                {
                    next = args.Length;
                }
                if (!args.Substring(previous, next - previous).TryParseKey(out toKey))
                {
                    yield break;
                }
                args = next < args.Length
                    ? args.Substring(next + 1)
                    : string.Empty;
            }
            foreach (var key in _history.Find(fromKey, toKey, args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries)))
            {
                yield return key.ToKeyString();
            }
        }
    }
}
