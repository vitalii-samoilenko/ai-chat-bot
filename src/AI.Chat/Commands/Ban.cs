using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Ban : ICommand
    {
        private readonly IModerator _moderator;

        public Ban(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            return _moderator.Ban(args.SplitArgs());
        }
    }
}
