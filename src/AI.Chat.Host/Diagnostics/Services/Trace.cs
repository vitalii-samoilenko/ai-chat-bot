using Microsoft.Extensions.Hosting;

namespace AI.Chat.Host.Services.Diagnostics
{
    public class Trace<TService> : IHostedService
        where TService : IHostedService
    {
        private static string ServiceName = $"{typeof(TService).Namespace}.{typeof(TService).Name}";

        private IHostedService _service;

        public Trace(TService service)
        {
            _service = service;
        }

        public async System.Threading.Tasks.Task StartAsync(System.Threading.CancellationToken cancellationToken)
        {
            using (var activity = AI.Chat.Host.Diagnostics.ActivitySources.Services.StartActivity($"{ServiceName}.{nameof(StartAsync)}"))
            {
                await _service.StartAsync(cancellationToken);
            }
        }

        public async System.Threading.Tasks.Task StopAsync(System.Threading.CancellationToken cancellationToken)
        {
            using (var activity = AI.Chat.Host.Diagnostics.ActivitySources.Services.StartActivity($"{ServiceName}.{nameof(StopAsync)}"))
            {
                await _service.StopAsync(cancellationToken);
            }
        }
    }
}
