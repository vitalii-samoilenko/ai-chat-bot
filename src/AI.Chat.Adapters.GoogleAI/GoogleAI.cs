namespace AI.Chat.Adapters
{
    public class GoogleAI : IAdapter
    {
        private readonly Options.GoogleAI.Adapter _options;
        private readonly global::GoogleAI.IClient _client;
        private readonly System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content> _contents;

        public GoogleAI(Options.GoogleAI.Adapter options, global::GoogleAI.IClient client)
            : this(options, client, new System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content>())
        {

        }
        public GoogleAI(Options.GoogleAI.Adapter options, global::GoogleAI.IClient client, System.Collections.Generic.TimeSeries<global::GoogleAI.Models.Content> contents)
        {
            _options = options;
            _client = client;
            _contents = contents;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            var contents = new System.Collections.Generic.List<global::GoogleAI.Models.Content>();
            var systemInstructionBuilder = new System.Text.StringBuilder();
            foreach (var content in _contents)
            {
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
            var request = new global::GoogleAI.Models.GenerateContentRequest
            {
                Contents = contents.ToArray(),
                SystemInstruction = 0 < systemInstructionBuilder.Length
                    ? new global::GoogleAI.Models.Content
                    {
                        Parts = new[]
                        {
                            new global::GoogleAI.Models.Part
                            {
                                Text = systemInstructionBuilder.Remove(0, 1).ToString()
                            }
                        }
                    }
                    : null,
                CachedContent = _options.CacheKey
            };
            var response = await _client.GenerateContentAsync(_options.Model, _options.ApiKey, request)
                .ConfigureAwait(false);
            return (response.Candidates[0].Content.Parts[0].Text, response.UsageMetadata.TotalTokenCount);
        }
    }
}
