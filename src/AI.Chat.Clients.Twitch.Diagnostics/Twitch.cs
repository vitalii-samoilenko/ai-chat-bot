namespace AI.Chat.Clients.Diagnostics
{
    public class Twitch<TTwitch> : ITwitch
        where TTwitch : ITwitch
    {
        private static string ClientName = $"{typeof(TTwitch).Namespace}.{typeof(TTwitch).Name}";

        private readonly ITwitch _client;

        public Twitch(TTwitch client)
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
