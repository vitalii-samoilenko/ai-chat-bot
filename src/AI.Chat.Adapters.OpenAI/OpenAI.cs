namespace AI.Chat.Adapters
{
    public class OpenAI : IAdapter
    {
        private readonly global::OpenAI.Chat.ChatClient _client;
        private readonly System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage> _history;

        public OpenAI(global::OpenAI.Chat.ChatClient client)
        {
            _client = client;
            _history = new System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage>();
        }

        public string AddInstruction(string content)
        {
            return _history.Add(
                    global::OpenAI.Chat.ChatMessage.CreateSystemMessage(
                        content))
                .Key
                .ToString();
        }
        public string AddMessage(string content)
        {
            return _history.Add(
                    global::OpenAI.Chat.ChatMessage.CreateUserMessage(
                        content))
                .Key
                .ToString();
        }
        public string AddReply(string content)
        {
            return _history.Add(
                    global::OpenAI.Chat.ChatMessage.CreateAssistantMessage(
                        content))
                .Key
                .ToString();
        }
        public bool TryGet(string key, out string content)
        {
            content = default;
            if (System.Collections.Generic.EntryKey.TryParse(key, out var entryKey)
                && _history.TryGet(entryKey, out var entry))
            {
                content = entry.Item.Content[0].Text;
                return true;
            }
            return false;
        }
        public void Remove(string key)
        {
            if (System.Collections.Generic.EntryKey.TryParse(key, out var entryKey))
            {
                _history.Remove(entryKey);
            }
        }
        public void RemoveAll()
        {
            _history.Clear();
        }
        public System.Collections.Generic.IEnumerable<string> Find(string fromKey, string toKey)
        {
            if (System.Collections.Generic.EntryKey.TryParse(fromKey, out var fromEntryKey)
                && System.Collections.Generic.EntryKey.TryParse(toKey, out var toEntryKey))
            {
                foreach (var entry in _history.Find(fromEntryKey, toEntryKey))
                {
                    yield return entry.Key.ToString();
                }
            }
        }
        public System.Collections.Generic.IEnumerable<string> FindAll()
        {
            foreach (var entry in _history.Entries)
            {
                yield return entry.Key.ToString();
            }
        }

        public async System.Threading.Tasks.Task<string> GetReplyAsync()
        {
            var completionResult = await _client.CompleteChatAsync(_history)
                .ConfigureAwait(false);
            return completionResult.Value.Content[0].Text;
        }
    }
}
