namespace AI.Chat.Commands.Twitch
{
    public class Cheerful : ICommand
    {
        private readonly Options.Twitch.Client _options;

        public Cheerful(Options.Twitch.Client options)
        {
            _options = options;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            if (System.Enum.TryParse(args, true, out Options.Twitch.WelcomeMode mode))
            {
                _options.Welcome.Mode = mode;
                yield return args;
            }
        }
    }
}
