namespace AI.Chat.Host.API.Services
{
    internal class Twitch : IHostedService
    {
        private Options.Twitch.Client _options;
        private Clients.Twitch _client;

        public Twitch(Options.Twitch.Client options, Clients.Twitch client)
        {
            _options = options;
            _client = client;
        }

        public async System.Threading.Tasks.Task StartAsync(System.Threading.CancellationToken cancellationToken)
        {
            try
            {
                await _client.StartAsync();
            }
            finally
            {
                Helpers.Save(_options);
            }
        }

        public System.Threading.Tasks.Task StopAsync(System.Threading.CancellationToken cancellationToken)
        {
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
