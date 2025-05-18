using System.Net.Http.Json;

namespace GoogleAI
{
    public interface IClient
    {
        System.Threading.Tasks.Task<Models.GenerateContentResponse> GenerateContentAsync(string model, string apiKey, Models.GenerateContentRequest request);
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
    }
}
