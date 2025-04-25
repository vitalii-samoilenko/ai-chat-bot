namespace AI.Chat.Commands
{
    public class Instruct : ICommand
    {
        private readonly IBot _bot;

        public Instruct(IBot bot)
        {
            _bot = bot;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _bot.Instruct(args);
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
