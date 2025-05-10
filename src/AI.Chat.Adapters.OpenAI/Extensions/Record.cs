namespace AI.Chat.Adapters.Extensions.OpenAI
{
    public static class Record
    {
        public static global::OpenAI.Chat.ChatMessage ToChatMessage(this AI.Chat.Record record)
        {
            foreach (var tag in record.Tags)
            {
                if (!tag.StartsWith(Constants.TagType))
                {
                    continue;
                }
                switch (tag.Substring(Constants.TagType.Length + 1))
                {
                    case Constants.TypeUser:
                        {
                            return global::OpenAI.Chat.ChatMessage.CreateUserMessage(record.Message);
                        }
                    case Constants.TypeModel:
                        {
                            return global::OpenAI.Chat.ChatMessage.CreateAssistantMessage(record.Message);
                        }
                }
            }
            return global::OpenAI.Chat.ChatMessage.CreateSystemMessage(record.Message);
        }
    }
}
