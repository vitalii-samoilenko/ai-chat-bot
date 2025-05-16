using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Get : ICommand
    {
        private readonly IHistory _history;

        public Get(IHistory history)
        {
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            if (args.TryParseKey(out var key)
                && _history.TryGet(key, out var record))
            {
                yield return args;
                yield return record.Message;
                foreach (var tag in record.Tags)
                {
                    yield return tag;
                }
            }
        }
    }
}
