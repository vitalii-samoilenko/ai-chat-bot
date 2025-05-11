using Microsoft.Extensions.Hosting;

namespace AI.Chat.Host.Services
{
    internal class Twitch : IHostedService
    {
        private AI.Chat.Options.Twitch.Client _options;
        private AI.Chat.Clients.ITwitch _client;

        public Twitch(Options.Twitch.Client options, AI.Chat.Clients.ITwitch client)
        {
            _options = options;
            _client = client;
        }

        public async System.Threading.Tasks.Task StartAsync(System.Threading.CancellationToken cancellationToken)
        {
            try
            {
                await _client.StartAsync()
                    .ConfigureAwait(false);
            }
            finally
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
        }

        public System.Threading.Tasks.Task StopAsync(System.Threading.CancellationToken cancellationToken)
        {
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
