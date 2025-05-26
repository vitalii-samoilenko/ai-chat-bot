using Microsoft.Extensions.Hosting;

namespace AI.Chat.Host.Services
{
    internal class Console : IHostedService
    {
        private AI.Chat.Clients.Console _client;

        public Console(AI.Chat.Clients.Console client)
        {
            _client = client;
        }

        public System.Threading.Tasks.Task StartAsync(System.Threading.CancellationToken cancellationToken)
        {
            _client.Start();
            return System.Threading.Tasks.Task.CompletedTask;
        }

        public System.Threading.Tasks.Task StopAsync(System.Threading.CancellationToken cancellationToken)
        {
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
