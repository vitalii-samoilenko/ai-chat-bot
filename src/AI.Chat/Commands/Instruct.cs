using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Instruct : ICommand
    {
        private readonly IHistory _history;

        public Instruct(IHistory history)
        {
            _history = history;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _history.AddSystemMessage(args);
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
