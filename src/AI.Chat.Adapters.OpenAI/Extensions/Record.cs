namespace AI.Chat.Adapters.Extensions.OpenAI
{
    public static class Record
    {
        public static global::OpenAI.Chat.ChatMessage ToChatMessage(this AI.Chat.Record record)
        {
            foreach (var tag in record.Tags)
            {
                if (!tag.StartsWith(Defaults.TagType + "=", System.StringComparison.OrdinalIgnoreCase))
                {
                    continue;
                }
                switch (tag.Substring(Defaults.TagType.Length + 1))
                {
                    case Defaults.TypeUser:
                        {
                            return global::OpenAI.Chat.ChatMessage.CreateUserMessage(record.Message);
                        }
                    case Defaults.TypeModel:
                        {
                            return global::OpenAI.Chat.ChatMessage.CreateAssistantMessage(record.Message);
                        }
                }
            }
            return global::OpenAI.Chat.ChatMessage.CreateSystemMessage(record.Message);
        }
    }
}
