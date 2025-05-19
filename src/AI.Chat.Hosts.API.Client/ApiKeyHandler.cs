namespace System.Net.Http.AI.Chat.Hosts.API
{
    public class ApiKeyHandler : DelegatingHandler
    {
        private readonly string _apiKey;

        public ApiKeyHandler(string apiKey)
        {
            _apiKey = apiKey;
        }

        protected override async Threading.Tasks.Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, Threading.CancellationToken cancellationToken)
        {
            request.Headers.Add("X-API-KEY", _apiKey);
            return await base.SendAsync(request, cancellationToken);
        }
    }
}
