namespace AI.Chat.Clients.Diagnostics
{
    public class Trace<TClient> : IClient
        where TClient : IClient
    {
        private static string ClientName = $"{typeof(TClient).Namespace}.{typeof(TClient).Name}";

        private readonly IClient _client;

        public Trace(TClient client)
        {
            _client = client;
        }

        public async System.Threading.Tasks.Task WelcomeAsync(string username, System.Func<System.DateTime, System.Threading.Tasks.Task> onAllowAsync, System.Func<System.DateTime, System.DateTime, System.Threading.Tasks.Task> onHoldAsync)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Clients.StartActivity($"{ClientName}.{nameof(WelcomeAsync)}"))
            {
                await _client.WelcomeAsync(username, onAllowAsync, onHoldAsync)
                    .ConfigureAwait(false);
            }
        }
        public async System.Threading.Tasks.Task ChatAsync(string username, string message, System.Func<System.DateTime, System.Threading.Tasks.Task> onAllowAsync, System.Func<System.DateTime, System.DateTime, System.Threading.Tasks.Task> onHoldAsync)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Clients.StartActivity($"{ClientName}.{nameof(ChatAsync)}"))
            {
                await _client.ChatAsync(username, message, onAllowAsync, onHoldAsync)
                    .ConfigureAwait(false);
            }
        }
    }
}
