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

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            if (!string.IsNullOrWhiteSpace(args))
            {
                var key = _history.AddSystemMessage(args);
                yield return key.ToKeyString();
            }
        }
    }
}
