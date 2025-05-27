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
            var tokens = args.SplitArgs();
            var tuples = new System.Collections.Generic.List<(string, System.TimeSpan)>();
            for (int i = 1; i < tokens.Length; i += 2)
            {
                if (!System.TimeSpan.TryParseExact(tokens[i], Defaults.TimeSpanFormat, null, out var timeout))
                {
                    continue;
                }
                var username = tokens[i - 1];
                tuples.Add((username, timeout));
            }
            if (0 < tuples.Count)
            {
                foreach ((var username, var until) in _moderator.Timeout(tuples))
                {
                    yield return username;
                    yield return until.ToKeyString();
                }
            }
        }
    }
}
