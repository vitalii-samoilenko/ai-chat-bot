namespace AI.Chat.Commands
{
    public class Timeout : ICommand
    {
        private readonly IModerator _moderator;

        public Timeout(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            var tokens = args.Split(' ');
            if (tokens.Length == 0 || tokens.Length % 2 == 1)
            {
                return System.Threading.Tasks.Task.CompletedTask;
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
            _moderator.Timeout(tuples.ToArray());
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
