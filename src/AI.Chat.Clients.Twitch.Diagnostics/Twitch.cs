namespace AI.Chat.Clients.Diagnostics
{
    public class Twitch : ITwitch
    {
        private static string ClientName = $"{typeof(Clients.Twitch).Namespace}.{typeof(Clients.Twitch).Name}";

        private readonly Clients.Twitch _client;

        public Twitch(Clients.Twitch client)
        {
            _client = client;
        }

        public async System.Threading.Tasks.Task StartAsync()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Clients.StartActivity($"{ClientName}.{nameof(StartAsync)}"))
            {
                 await _client.StartAsync()
                    .ConfigureAwait(false);
            }
        }
    }
}
