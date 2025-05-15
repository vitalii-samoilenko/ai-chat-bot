using AI.Chat.Extensions;

namespace AI.Chat.Commands
{
    public class Allow : ICommand
    {
        private readonly IModerator _moderator;

        public Allow(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            System.Collections.Generic.IEnumerable<System.DateTime> allowedKeys = null;
            if (args == Constants.ArgsAll)
            {
                allowedKeys = _moderator.AllowAll();
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
                    _moderator.Allow(keys.ToArray());
                }
                allowedKeys = keys;
            };
            foreach (var key in allowedKeys)
            {
                yield return key.ToKeyString();
            }
        }
    }
}
