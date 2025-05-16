using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Timeout : ICommand
    {
        private readonly IModerator _moderator;

        public Timeout(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var tokens = args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            if (tokens.Length == 0 || tokens.Length % 2 == 1)
            {
                yield break;
            }
            var tuples = new System.Collections.Generic.List<(string, System.TimeSpan)>();
            for (int i = 0; i < tokens.Length; i += 2)
            {
                if (!System.TimeSpan.TryParseExact(tokens[i + 1], Constants.TimeSpanFormat, null, out var timeout))
                {
                    continue;
                }
                var username = tokens[i];
                tuples.Add((username, timeout));
            }
            foreach ((var username, var until) in _moderator.Timeout(tuples.ToArray()))
            {
                yield return username;
                yield return until.ToKeyString();
            }
        }
    }
}
