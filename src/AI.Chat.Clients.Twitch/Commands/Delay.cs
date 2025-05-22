namespace AI.Chat.Commands.Twitch
{
    public class Delay : ICommand
    {
        private readonly Options.Twitch.Client _options;

        public Delay(Options.Twitch.Client options)
        {
            _options = options;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            if (System.TimeSpan.TryParseExact(args, Constants.TimeSpanFormat, null, out var delay))
            {
                _options.Delay = delay;
                yield return true.ToString();
            }
        }
    }
}
