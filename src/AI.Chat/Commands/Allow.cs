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

        public async System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            System.Func<System.Threading.Tasks.Task> callback;
            if (args == Constants.ArgsAll)
            {
                callback = _moderator.AllowAll();
            }
            else
            {
                var keys = new System.Collections.Generic.List<System.DateTime>();
                foreach (var arg in args.Split(' '))
                {
                    if (!arg.TryParseKey(out var key))
                    {
                        continue;
                    }
                    keys.Add(key);
                }
                callback = _moderator.Allow(keys.ToArray());
            }
            await callback()
                .ConfigureAwait(false);
        }
    }
}
