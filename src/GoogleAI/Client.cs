using System.Net.Http.Json;

namespace GoogleAI
{
    public interface IClient
    {
        System.Threading.Tasks.Task<Models.GenerateContentResponse> GenerateContentAsync(string model, Models.GenerateContentRequest request);
        System.Threading.Tasks.Task<Models.CachedContentsResponse> CreateCachedCotents(Models.CachedContentsRequest request);
        System.Threading.Tasks.Task<bool> TryDeleteCachedContents(string name);
    }

    public class Client : IClient
    {
        private readonly System.Net.Http.HttpClient _httpClient;

        public Client(System.Net.Http.HttpClient httpClient)
        {
            _httpClient = httpClient;
        }

        public async System.Threading.Tasks.Task<Models.GenerateContentResponse> GenerateContentAsync(string model, Models.GenerateContentRequest request)
        {
            var httpResponse = await _httpClient.SendAsync(
                new System.Net.Http.HttpRequestMessage
                {
                    Method = System.Net.Http.HttpMethod.Post,
                    RequestUri = new System.Uri(
                        $"models/{model}:generateContent",
                        System.UriKind.Relative),
                    Content = System.Net.Http.Json.JsonContent.Create(
                        request)
                })
                .ConfigureAwait(false);
            httpResponse.EnsureSuccessStatusCode();
            var response = await httpResponse.Content
                .ReadFromJsonAsync<Models.GenerateContentResponse>()
                .ConfigureAwait(false);
            return response;
        }
        public async System.Threading.Tasks.Task<Models.CachedContentsResponse> CreateCachedCotents(Models.CachedContentsRequest request)
        {
            var httpResponse = await _httpClient.SendAsync(
                new System.Net.Http.HttpRequestMessage
                {
                    Method = System.Net.Http.HttpMethod.Post,
                    RequestUri = new System.Uri(
                        "cachedContents",
                        System.UriKind.Relative),
                    Content = System.Net.Http.Json.JsonContent.Create(
                        request)
                })
                .ConfigureAwait(false);
            httpResponse.EnsureSuccessStatusCode();
            var response = await httpResponse.Content
                .ReadFromJsonAsync<Models.CachedContentsResponse>()
                .ConfigureAwait(false);
            return response;
        }
        public async System.Threading.Tasks.Task<bool> TryDeleteCachedContents(string name)
        {
            var httpResponse = await _httpClient.SendAsync(
                new System.Net.Http.HttpRequestMessage
                {
                    Method = System.Net.Http.HttpMethod.Delete,
                    RequestUri = new System.Uri(
                        name,
                        System.UriKind.Relative)
                })
                .ConfigureAwait(false);
            return httpResponse.IsSuccessStatusCode;
        }
    }
}
