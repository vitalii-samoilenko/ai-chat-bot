using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Mod : ICommand
    {
        private readonly IModerator _moderator;

        public Mod(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            return _moderator.Moderate(args.SplitArgs());
        }
    }
}
