using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Unwelcome : ICommand
    {
        private readonly IModerator _moderator;

        public Unwelcome(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            return _moderator.Unwelcome(args.SplitArgs());
        }
    }
}
