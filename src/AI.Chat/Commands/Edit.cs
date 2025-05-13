using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Edit : ICommand
    {
        private readonly IHistory _history;

        public Edit(IHistory history)
        {
            _history = history;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            var previous = 0;
            var next = args.IndexOf(' ', previous);
            if (next < 0)
            {
                return System.Threading.Tasks.Task.CompletedTask;
            }
            if (args.Substring(previous, next - previous).TryParseKey(out var key))
            {
                _history.TryEdit(key, args.Substring(next + 1));
            }
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
