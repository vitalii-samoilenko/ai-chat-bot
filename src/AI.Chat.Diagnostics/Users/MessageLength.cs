namespace AI.Chat.Users.Diagnostics
{
    public class MessageLength<TUser> : IUser
        where TUser : IUser
    {
        private readonly IUser _user;

        public MessageLength(TUser user)
        {
            _user = user;
        }

        public async System.Threading.Tasks.Task WelcomeAsync(string username, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync)
        {
            await _user.WelcomeAsync(username, onAllowAsync, onHoldAsync)
                .ConfigureAwait(false);
        }
        public async System.Threading.Tasks.Task ChatAsync(string username, string message, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync)
        {
            AI.Chat.Diagnostics.Meters.MessageLength.Record(message.Length,
                new System.Collections.Generic.KeyValuePair<string, object>("user.name", username));
            await _user.ChatAsync(username, message, onAllowAsync, onHoldAsync)
                .ConfigureAwait(false);
        }
    }
}
