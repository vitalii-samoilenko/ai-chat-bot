using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Seek : ICommand
    {
        private readonly IHistory _history;

        public Seek(IHistory history)
        {
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var fromKey = System.DateTime.MinValue;
            var toKey = System.DateTime.MaxValue;
            if (args.StartsWith(Constants.ArgsAll, System.StringComparison.OrdinalIgnoreCase))
            {
                args = args.Substring(Constants.ArgsAll.Length);
            }
            else if (!args.ExtractToken(out args).TryParseKey(out fromKey)
                || !args.ExtractToken(out args).TryParseKey(out toKey))
            {
                yield break;
            }
            if (int.TryParse(args.ExtractToken(out var remainder), out var pageSize))
            {
                args = remainder;
            }
            else
            {
                pageSize = 1000;
            }
            foreach (var key in _history.Find(fromKey, toKey, args.SplitArgs()))
            {
                if (_history.TryGet(key, out var record))
                {
                    if (--pageSize < 0)
                    {
                        yield return Constants.Etc;
                        yield break;
                    }
                    yield return key.ToKeyString();
                    yield return record.Message;
                    foreach (var tag in record.Tags)
                    {
                        yield return tag;
                    }
                }
            }
        }
    }
}
