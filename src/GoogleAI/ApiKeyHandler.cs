namespace System.Net.Http.GoogleAI
{
    public class ApiKeyHandler : DelegatingHandler
    {
        private readonly string _apiKeyQuery;

        public ApiKeyHandler(string apiKey)
        {
            _apiKeyQuery = $"key={apiKey}";
        }

        protected override async Threading.Tasks.Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, Threading.CancellationToken cancellationToken)
        {
            var uri = request.RequestUri;
            request.RequestUri = new Uri(
                new Text.StringBuilder(uri.AbsoluteUri)
                    .Append(string.IsNullOrWhiteSpace(uri.Query)
                        ? '?'
                        : '&')
                    .Append(_apiKeyQuery)
                    .ToString(),
                UriKind.Absolute);
            return await base.SendAsync(request, cancellationToken);
        }
    }
}
