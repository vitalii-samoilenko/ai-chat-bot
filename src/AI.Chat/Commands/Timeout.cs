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
                var username = tokens[i];
                if (System.TimeSpan.TryParseExact(tokens[i + 1], "HH:mm:ss", null, out var timeout))
                {
                    tuples.Add((username, timeout));
                }
            }
            _moderator.Timeout(tuples.ToArray());
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
