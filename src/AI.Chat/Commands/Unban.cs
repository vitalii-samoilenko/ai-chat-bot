namespace AI.Chat.Commands
{
    public class Unban : ICommand
    {
        private readonly IModerator _moderator;

        public Unban(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            return _moderator.Unban(args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries));
        }
    }
}
