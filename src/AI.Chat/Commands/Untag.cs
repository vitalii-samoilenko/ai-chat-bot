using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Untag : ICommand
    {
        private readonly IHistory _history;

        public Untag(IHistory history)
        {
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var tokens = args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            if (tokens.Length < 2)
            {
                yield break;
            }
            var tag = tokens[0];
            var keys = new System.Collections.Generic.List<System.DateTime>();
            for (int i = 1; i < tokens.Length; ++i)
            {
                var token = tokens[i];
                if (!token.TryParseKey(out var key))
                {
                    continue;
                }
                keys.Add(key);
            }
            if (0 < keys.Count)
            {
                foreach (var key in _history.Untag(tag, keys.ToArray()))
                {
                    yield return key.ToKeyString();
                }
            }
        }
    }
}
