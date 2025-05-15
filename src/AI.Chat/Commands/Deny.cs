using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Deny : ICommand
    {
        private readonly IModerator _moderator;

        public Deny(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            System.Collections.Generic.IEnumerable<System.DateTime> deniedKeys = null;
            if (args == Constants.ArgsAll)
            {
                deniedKeys = _moderator.DenyAll();
            }
            else
            {
                var keys = new System.Collections.Generic.List<System.DateTime>();
                foreach (var arg in args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries))
                {
                    if (!arg.TryParseKey(out var key))
                    {
                        continue;
                    }
                    keys.Add(key);
                }
                if (0 < keys.Count)
                {
                    _moderator.Deny(keys.ToArray());
                }
                deniedKeys = keys;
            }
            foreach (var key in deniedKeys)
            {
                yield return key.ToKeyString();
            }
        }
    }
}
