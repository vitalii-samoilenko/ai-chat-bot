using System.Net.Http.Json;

namespace TwitchLib.Client
{
    namespace Interfaces
    {
        public interface IAuthClient
        {
            System.Threading.Tasks.Task<bool> ValidateTokenAsync(
                string accessToken);
            System.Threading.Tasks.Task<string> RefreshTokenAsync(
                string clientId,
                string clientSecret,
                string refreshToken);
            System.Threading.Tasks.Task<(string accessToken, string refreshToken)> IssueTokenAsync(
                string clientId,
                string deviceCode,
                string scopes);
            System.Threading.Tasks.Task<(string deviceCode, string uri)> RequestAccessAsync(
                string clientId,
                string scopes);
        }
    }

    public class AuthClient : Interfaces.IAuthClient
    {
        private class TokenResponse
        {
            public string access_token { get; set; }
            public string refresh_token { get; set; }
        }
        private class DeviceResponse
        {
            public string device_code { get; set; }
            public string verification_uri { get; set; }
        }

        private readonly System.Net.Http.HttpClient _httpClient;

        public AuthClient(System.Net.Http.HttpClient httpClient)
        {
            _httpClient = httpClient;
        }

        public async System.Threading.Tasks.Task<bool> ValidateTokenAsync(string accessToken)
        {
            var httpResponse = await _httpClient.SendAsync(
                    new System.Net.Http.HttpRequestMessage
                    {
                        Method = System.Net.Http.HttpMethod.Get,
                        RequestUri = new System.Uri(
                            "validate",
                            System.UriKind.Relative),
                        Headers =
                        {
                            { "Authorization", $"OAuth {accessToken}" }
                        }
                    })
                .ConfigureAwait(false);
            return httpResponse.IsSuccessStatusCode;
        }

        public async System.Threading.Tasks.Task<string> RefreshTokenAsync(string clientId, string clientSecret, string refreshToken)
        {
            var httpResponse = await _httpClient.SendAsync(
                    new System.Net.Http.HttpRequestMessage
                    {
                        Method = System.Net.Http.HttpMethod.Post,
                        RequestUri = new System.Uri(
                            "token",
                            System.UriKind.Relative),
                        Content = new System.Net.Http.FormUrlEncodedContent(
                            new System.Collections.Generic.Dictionary<string, string>
                            {
                                { "client_id", clientId },
                                { "client_secret", clientSecret },
                                { "grant_type", "refresh_token" },
                                { "refresh_token", refreshToken }
                            })
                    })
                .ConfigureAwait(false);
            httpResponse.EnsureSuccessStatusCode();
            var tokenResponse = await httpResponse.Content
                .ReadFromJsonAsync<TokenResponse>()
                .ConfigureAwait(false);
            return tokenResponse.access_token;
        }

        public async System.Threading.Tasks.Task<(string accessToken, string refreshToken)> IssueTokenAsync(string clientId, string deviceCode, string scopes)
        {
            var httpResponse = await _httpClient.SendAsync(
                    new System.Net.Http.HttpRequestMessage
                    {
                        Method = System.Net.Http.HttpMethod.Post,
                        RequestUri = new System.Uri(
                            $"token?client_id={clientId}&grant_type=urn:ietf:params:oauth:grant-type:device_code&device_code={deviceCode}&scopes={scopes}",
                            System.UriKind.Relative)
                    })
                .ConfigureAwait(false);
            httpResponse.EnsureSuccessStatusCode();
            var tokenResponse = await httpResponse.Content
                .ReadFromJsonAsync<TokenResponse>()
                .ConfigureAwait(false);
            return (tokenResponse.access_token, tokenResponse.refresh_token);
        }

        public async System.Threading.Tasks.Task<(string deviceCode, string uri)> RequestAccessAsync(string clientId, string scopes)
        {
            var httpResponse = await _httpClient.SendAsync(
                    new System.Net.Http.HttpRequestMessage
                    {
                        Method = System.Net.Http.HttpMethod.Post,
                        RequestUri = new System.Uri(
                            $"device?client_id={clientId}&scopes={scopes}",
                            System.UriKind.Relative)
                    })
                .ConfigureAwait(false);
            httpResponse.EnsureSuccessStatusCode();
            var deviceResponse = await httpResponse.Content
                .ReadFromJsonAsync<DeviceResponse>()
                .ConfigureAwait(false);
            return (deviceResponse.device_code, deviceResponse.verification_uri);
        }
    }

    public class DummyAuthClient : Interfaces.IAuthClient
    {
        public System.Threading.Tasks.Task<(string accessToken, string refreshToken)> IssueTokenAsync(string clientId, string deviceCode, string scopes)
            => System.Threading.Tasks.Task.FromResult(("accessToken", "refreshToken"));

        public System.Threading.Tasks.Task<string> RefreshTokenAsync(string clientId, string clientSecret, string refreshToken)
            => System.Threading.Tasks.Task.FromResult("accessToken");

        public System.Threading.Tasks.Task<(string deviceCode, string uri)> RequestAccessAsync(string clientId, string scopes)
            => System.Threading.Tasks.Task.FromResult(("deviceCode", "uri"));

        public System.Threading.Tasks.Task<bool> ValidateTokenAsync(string accessToken)
            => System.Threading.Tasks.Task.FromResult(true);
    }
}