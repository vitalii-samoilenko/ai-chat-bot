namespace AI.Chat.Commands
{
    public class Remove : ICommand
    {
        private readonly Options.User _options;
        private readonly IBot _bot;

        public Remove(Options.User options, IBot bot)
        {
            _options = options;
            _bot = bot;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            if (args == "all")
            {
                _bot.RemoveAll();
                _bot.Instruct(_options.Context);
            }
            else
            {
                _bot.Remove(args.Split(' '));
            }
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
