using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Remove : ICommand
    {
        private readonly IHistory _history;

        public Remove(IHistory history)
        {
            _history = history;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            if (args == Constants.ArgsAll)
            {
                _history.Clear();
            }
            else
            {
                var keys = new System.Collections.Generic.List<System.DateTime>();
                foreach (var arg in args.Split(' '))
                {
                    if (!arg.TryParseKey(out var key))
                    {
                        continue;
                    }
                    keys.Add(key);
                }
                _history.Remove(keys.ToArray());
            }
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
