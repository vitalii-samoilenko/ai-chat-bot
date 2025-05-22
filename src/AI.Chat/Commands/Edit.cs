using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Edit : ICommand
    {
        private readonly IHistory _history;

        public Edit(IHistory history)
        {
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var previous = 0;
            var next = args.IndexOf(' ', previous);
            if (next < 0)
            {
                yield break;
            }
            var keyArg = args.Substring(previous, next - previous);
            var message = args.Substring(next + 1);
            if (keyArg.TryParseKey(out var key)
                && !string.IsNullOrWhiteSpace(message)
                && _history.TryEdit(key, message))
            {
                yield return true.ToString();
            }
        }
    }
}
