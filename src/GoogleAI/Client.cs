using System.Net.Http.Json;

namespace GoogleAI
{
    public interface IClient
    {
        System.Threading.Tasks.Task<Models.GenerateContentResponse> GenerateContentAsync(string model, string apiKey, Models.GenerateContentRequest request);
        System.Threading.Tasks.Task<Models.CachedContentsResponse> CreateCachedCotents(string apiKey, Models.CachedContentsRequest request);
        System.Threading.Tasks.Task DeleteCachedContents(string name, string apiKey);
    }

    public class Client : IClient
    {
        private readonly System.Net.Http.HttpClient _httpClient;

        public Client(System.Net.Http.HttpClient httpClient)
        {
            _httpClient = httpClient;
        }

        public async System.Threading.Tasks.Task<Models.GenerateContentResponse> GenerateContentAsync(string model, string apiKey, Models.GenerateContentRequest request)
        {
            var httpResponse = await _httpClient.SendAsync(
                new System.Net.Http.HttpRequestMessage
                {
                    Method = System.Net.Http.HttpMethod.Post,
                    RequestUri = new System.Uri(
                        $"models/{model}:generateContent"
                        + $"?key={apiKey}",
                        System.UriKind.Relative),
                    Content = System.Net.Http.Json.JsonContent.Create(
                        request)
                })
                .ConfigureAwait(false);
            if (!httpResponse.IsSuccessStatusCode)
            {
                throw new System.Net.Http.HttpRequestException(
                    $"Failed to generate content: {httpResponse.StatusCode}");
            }
            var response = await httpResponse.Content
                .ReadFromJsonAsync<Models.GenerateContentResponse>()
                .ConfigureAwait(false);
            return response;
        }
        public async System.Threading.Tasks.Task<Models.CachedContentsResponse> CreateCachedCotents(string apiKey, Models.CachedContentsRequest request)
        {
            var httpResponse = await _httpClient.SendAsync(
                new System.Net.Http.HttpRequestMessage
                {
                    Method = System.Net.Http.HttpMethod.Post,
                    RequestUri = new System.Uri(
                        "cachedContents"
                        + $"?key={apiKey}",
                        System.UriKind.Relative),
                    Content = System.Net.Http.Json.JsonContent.Create(
                        request)
                })
                .ConfigureAwait(false);
            if (!httpResponse.IsSuccessStatusCode)
            {
                throw new System.Net.Http.HttpRequestException(
                    $"Failed to create cached contents: {httpResponse.StatusCode}");
            }
            var response = await httpResponse.Content
                .ReadFromJsonAsync<Models.CachedContentsResponse>()
                .ConfigureAwait(false);
            return response;
        }
        public async System.Threading.Tasks.Task DeleteCachedContents(string name, string apiKey)
        {
            var httpResponse = await _httpClient.SendAsync(
                new System.Net.Http.HttpRequestMessage
                {
                    Method = System.Net.Http.HttpMethod.Delete,
                    RequestUri = new System.Uri(
                        name
                        + $"?key={apiKey}",
                        System.UriKind.Relative)
                })
                .ConfigureAwait(false);
            if (!httpResponse.IsSuccessStatusCode)
            {
                throw new System.Net.Http.HttpRequestException(
                    $"Failed to delete cached contents: {httpResponse.StatusCode}");
            }
        }
    }
}
