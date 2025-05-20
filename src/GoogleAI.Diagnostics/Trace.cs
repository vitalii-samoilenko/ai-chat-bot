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

        public async System.Threading.Tasks.Task<Models.GenerateContentResponse> GenerateContentAsync(string model, Models.GenerateContentRequest request)
        {
            using (var activity = ActivitySources.Client.StartActivity($"{ClientName}.{nameof(GenerateContentAsync)}"))
            {
                return await _client.GenerateContentAsync(model, request)
                    .ConfigureAwait(false);
            }
        }
        public async System.Threading.Tasks.Task<Models.CachedContentsResponse> CreateCachedCotents(Models.CachedContentsRequest request)
        {
            using (var activity = ActivitySources.Client.StartActivity($"{ClientName}.{nameof(CreateCachedCotents)}"))
            {
                return await _client.CreateCachedCotents(request)
                    .ConfigureAwait(false);
            }
        }
        public async System.Threading.Tasks.Task<bool> TryDeleteCachedContents(string name)
        {
            using (var activity = ActivitySources.Client.StartActivity($"{ClientName}.{nameof(TryDeleteCachedContents)}"))
            {
                return await _client.TryDeleteCachedContents(name)
                    .ConfigureAwait(false);
            }
        }
    }
}
