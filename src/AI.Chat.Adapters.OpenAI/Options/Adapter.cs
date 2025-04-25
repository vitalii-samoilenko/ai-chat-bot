namespace AI.Chat.Options.OpenAI
{
    public class Adapter : Options.Adapter
    {
        public string ApiKey { get; set; }
        public string Model { get; set; }
        public global::OpenAI.OpenAIClientOptions Client { get; set; }
    }
}
