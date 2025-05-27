namespace AI.Chat.Adapters
{
    public class GoogleAI : IAdapter, System.IDisposable
    {
        private readonly Options.GoogleAI.Adapter _options;
        private readonly global::GoogleAI.IClient _client;
        private readonly System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content> _contents;

        private bool _disposed;

        public GoogleAI(Options.GoogleAI.Adapter options, global::GoogleAI.IClient client)
            : this(options, client, new System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content>())
        {

        }
        public GoogleAI(Options.GoogleAI.Adapter options, global::GoogleAI.IClient client, System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content> contents)
        {
            _options = options;
            _client = client;
            _contents = contents;

            _disposed = false;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            var until = System.DateTime.UtcNow + _options.Cache.Slide;
            var contents = new System.Collections.Generic.List<global::GoogleAI.Models.Content>();
            var systemInstructionBuilder = new System.Text.StringBuilder();
            var lastKey = System.DateTime.MinValue;
        init:
            if (_options.Cache.Until < until)
            {
                _options.Cache.Key = System.DateTime.MinValue;
                if (!string.IsNullOrWhiteSpace(_options.Cache.Name))
                {
                    await _client.TryDeleteCachedContents(_options.Cache.Name)
                        .ConfigureAwait(false);
                    _options.Cache.Name = null;
                }
            }
            foreach (var entry in _contents.Find(_options.Cache.Key, System.DateTime.MaxValue))
            {
                lastKey = entry.Key;
                var content = entry.Value;
                if (AI.Chat.Constants.TypeSystem.Equals(content.Role, System.StringComparison.OrdinalIgnoreCase))
                {
                    systemInstructionBuilder.Append(' ');
                    systemInstructionBuilder.Append(content.Parts[0].Text);
                }
                else
                {
                    contents.Add(content);
                }
            }
            if (!(0 < contents.Count))
            {
                _options.Cache.Until = System.DateTime.MinValue;
                goto init;
            }
            var request = new global::GoogleAI.Models.GenerateContentRequest
            {
                Contents = contents,
                SystemInstruction = 0 < systemInstructionBuilder.Length
                    ? new global::GoogleAI.Models.Content
                    {
                        Parts = new System.Collections.Generic.List<global::GoogleAI.Models.Part>
                        {
                            new global::GoogleAI.Models.Part
                            {
                                Text = systemInstructionBuilder.Remove(0, 1).ToString()
                            }
                        }
                    }
                    : null,
                CachedContent = _options.Cache.Name,
                GenerationConfig = _options.Model.Generation
            };
            var response = await _client.GenerateContentAsync(_options.Model.Name, request)
                .ConfigureAwait(false);
            if (_options.Cache.Until < until
                && !(response.UsageMetadata.PromptTokenCount < _options.Cache.Tokens))
            {
                var cacheRequest = new global::GoogleAI.Models.CachedContentsRequest
                {
                    Model = $"models/{_options.Model.Name}",
                    Contents = request.Contents,
                    SystemInstruction = request.SystemInstruction,
                    Ttl = _options.Cache.Ttl
                };
                var cacheResponse = await _client.CreateCachedCotents(cacheRequest)
                    .ConfigureAwait(false);
                _options.Cache.Key = lastKey + System.TimeSpan.FromTicks(1);
                _options.Cache.Name = cacheResponse.Name;
                _options.Cache.Until = cacheResponse.ExpireTime;
            }
            return (response.Candidates[0].Content.Parts[0].Text, response.UsageMetadata.TotalTokenCount);
        }

        public void Dispose()
        {
            Dispose(true);
            System.GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                if (!string.IsNullOrWhiteSpace(_options.Cache.Name))
                {
                    _client.TryDeleteCachedContents(_options.Cache.Name)
                        .ConfigureAwait(false)
                        .GetAwaiter()
                        .GetResult();
                }
                _disposed = true;
            }
        }

        ~GoogleAI()
        {
            Dispose(false);
        }
    }
}
