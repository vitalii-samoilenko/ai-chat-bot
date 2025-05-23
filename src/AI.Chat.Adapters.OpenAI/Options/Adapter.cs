namespace AI.Chat.Options.OpenAI
{
    public class Model
    {
        public string Name { get; set; }
        public global::OpenAI.Chat.ChatCompletionOptions Completion { get; set; }
    }
    public class Client : global::OpenAI.OpenAIClientOptions
    {
        public string ApiKey { get; set; }
    }
    public class Adapter : Options.Adapter
    {
        public Client Client { get; set; }
        public Model Model { get; set; }
    }
}
