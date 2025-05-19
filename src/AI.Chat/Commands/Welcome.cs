using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Welcome : ICommand
    {
        private readonly IModerator _moderator;

        public Welcome(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            return _moderator.Welcome(args.SplitArgs());
        }
    }
}
