namespace AI.Chat.Commands
{
    public class Promote : ICommand
    {
        private readonly IModerator _moderator;

        public Promote(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            return _moderator.Promote(args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries));
        }
    }
}
