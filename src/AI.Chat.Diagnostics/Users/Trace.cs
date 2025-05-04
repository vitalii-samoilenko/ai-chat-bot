namespace AI.Chat.Users.Diagnostics
{
    public class Trace<TUser> : IUser
        where TUser : IUser
    {
        private static string UserName = $"{typeof(TUser).Namespace}.{typeof(TUser).Name}";

        private readonly IUser _user;

        public Trace(TUser user)
        {
            _user = user;
        }

        public async System.Threading.Tasks.Task WelcomeAsync(string username, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Users.StartActivity($"{UserName}.{nameof(WelcomeAsync)}"))
            {
                await _user.WelcomeAsync(username, onAllowAsync, onHoldAsync)
                    .ConfigureAwait(false);
            }
        }
        public async System.Threading.Tasks.Task ChatAsync(string username, string message, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Users.StartActivity($"{UserName}.{nameof(ChatAsync)}"))
            {
                await _user.ChatAsync(username, message, onAllowAsync, onHoldAsync)
                    .ConfigureAwait(false);
            }
        }
    }
}
