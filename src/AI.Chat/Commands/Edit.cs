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
            if (args.ExtractToken(out args).TryParseKey(out var key)
                && !string.IsNullOrWhiteSpace(args)
                && _history.TryEdit(key, args))
            {
                yield return true.ToString();
            }
        }
    }
}
