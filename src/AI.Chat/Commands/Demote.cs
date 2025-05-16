namespace AI.Chat.Commands
{
    public class Demote : ICommand
    {
        private readonly IModerator _moderator;

        public Demote(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            return _moderator.Demote(args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries));
        }
    }
}
