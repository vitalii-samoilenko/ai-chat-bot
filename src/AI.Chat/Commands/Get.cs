using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Get : ICommand
    {
        [System.Flags]
        internal enum Format
        {
            Message = 0b01,
            Tags = 0b10,
            Both = Message | Tags
        }

        private readonly IHistory _history;

        public Get(IHistory history)
        {
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var previous = 0;
            var next = args.IndexOf(' ', previous);
            if (next < 0)
            {
                next = args.Length;
            }
            var format = Format.Message;
            if (args.Substring(previous, next - previous).TryParseKey(out var key)
                && _history.TryGet(key, out var record)
                && (!(next < args.Length)
                    || System.Enum.TryParse(args.Substring(next + 1), true, out format)))
            {
                if (0 < (format & Format.Message))
                {
                    yield return record.Message;
                }
                if (0 < (format & Format.Tags))
                {
                    foreach (var tag in record.Tags)
                    {
                        yield return tag;
                    }
                }
            }
        }
    }
}
