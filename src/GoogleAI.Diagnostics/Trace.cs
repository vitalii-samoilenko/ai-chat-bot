namespace GoogleAI.Diagnostics
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

        public async System.Threading.Tasks.Task<Models.GenerateContentResponse> GenerateContentAsync(string model, string apiKey, Models.GenerateContentRequest request)
        {
            using (var activity = ActivitySources.Client.StartActivity($"{ClientName}.{nameof(GenerateContentAsync)}"))
            {
                return await _client.GenerateContentAsync(model, apiKey, request)
                    .ConfigureAwait(false);
            }
        }
    }
}
