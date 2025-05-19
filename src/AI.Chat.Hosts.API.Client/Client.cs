using System.Net.Http.Json;

namespace AI.Chat.Hosts.API
{
    public interface IClient
    {
        System.Threading.Tasks.Task<System.Collections.Generic.IEnumerable<string>> ExecuteAsync(string command, string args);
    }

    public class Client : IClient
    {
        private readonly System.Net.Http.HttpClient _httpClient;

        public Client(System.Net.Http.HttpClient httpClient)
        {
            _httpClient = httpClient;
        }

        public async System.Threading.Tasks.Task<System.Collections.Generic.IEnumerable<string>> ExecuteAsync(string command, string args)
        {
            var httpResponse = await _httpClient.SendAsync(
                new System.Net.Http.HttpRequestMessage
                {
                    Method = System.Net.Http.HttpMethod.Get,
                    RequestUri = new System.Uri(
                        $"{command}/execute"
                        + $"?args={args}",
                        System.UriKind.Relative)
                })
                .ConfigureAwait(false);
            if (!httpResponse.IsSuccessStatusCode)
            {
                throw new System.Net.Http.HttpRequestException(
                    $"Failed to execute command: {httpResponse.StatusCode}");
            }
            var response = await httpResponse.Content
                .ReadFromJsonAsync<System.Collections.Generic.List<string>>()
                .ConfigureAwait(false);
            return response;
        }
    }
}
