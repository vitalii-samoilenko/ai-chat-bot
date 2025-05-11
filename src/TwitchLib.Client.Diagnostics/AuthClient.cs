namespace TwitchLib.Client.Diagnostics
{
    public class AuthClient<TAuthClient> : Interfaces.IAuthClient
        where TAuthClient : Interfaces.IAuthClient
    {
        private static string ClientName = $"{typeof(TAuthClient).Namespace}.{typeof(TAuthClient).Name}";

        private readonly Interfaces.IAuthClient _client;

        public AuthClient(TAuthClient client)
        {
            _client = client;
        }

        public async System.Threading.Tasks.Task<bool> ValidateTokenAsync(string accessToken)
        {
            using (var activity = ActivitySources.Client.StartActivity($"{ClientName}.{nameof(ValidateTokenAsync)}"))
            {
                return await _client.ValidateTokenAsync(accessToken)
                    .ConfigureAwait(false);
            }
        }

        public async System.Threading.Tasks.Task<string> RefreshTokenAsync(string clientId, string clientSecret, string refreshToken)
        {
            using (var activity = ActivitySources.Client.StartActivity($"{ClientName}.{nameof(RefreshTokenAsync)}"))
            {
                return await _client.RefreshTokenAsync(clientId, clientSecret, refreshToken)
                    .ConfigureAwait(false);
            }
        }

        public async System.Threading.Tasks.Task<(string accessToken, string refreshToken)> IssueTokenAsync(string clientId, string deviceCode, string scopes)
        {
            using (var activity = ActivitySources.Client.StartActivity($"{ClientName}.{nameof(IssueTokenAsync)}"))
            {
                return await _client.IssueTokenAsync(clientId, deviceCode, scopes)
                    .ConfigureAwait(false);
            }
        }

        public async System.Threading.Tasks.Task<(string deviceCode, string uri)> RequestAccessAsync(string clientId, string scopes)
        {
            using (var activity = ActivitySources.Client.StartActivity($"{ClientName}.{nameof(RequestAccessAsync)}"))
            {
                return await _client.RequestAccessAsync(clientId, scopes)
                    .ConfigureAwait(false);
            }
        }
    }
}