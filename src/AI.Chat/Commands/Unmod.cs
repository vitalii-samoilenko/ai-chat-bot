using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Unmod : ICommand
    {
        private readonly IModerator _moderator;

        public Unmod(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            return _moderator.Unmoderate(args.SplitArgs());
        }
    }
}
