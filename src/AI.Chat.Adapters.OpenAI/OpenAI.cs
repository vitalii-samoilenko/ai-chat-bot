namespace AI.Chat.Adapters
{
    public class OpenAI : IAdapter
    {
        private readonly global::OpenAI.Chat.ChatClient _client;
        private readonly System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage> _messages;

        public OpenAI(global::OpenAI.Chat.ChatClient client)
            : this(client, new System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage>())
        {

        }
        public OpenAI(global::OpenAI.Chat.ChatClient client, System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage> messages)
        {
            _client = client;
            _messages = messages;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            var completionResult = await _client.CompleteChatAsync(_messages)
                .ConfigureAwait(false);
            return (completionResult.Value.Content[0].Text, completionResult.Value.Usage.TotalTokenCount);
        }
    }
}
