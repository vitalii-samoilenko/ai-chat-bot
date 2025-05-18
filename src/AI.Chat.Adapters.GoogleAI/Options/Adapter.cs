namespace AI.Chat.Options.GoogleAI
{
    public class Client
    {
        public System.Uri BaseAddress { get; set; }
    }
    public class Adapter : Options.Adapter
    {
        public string ApiKey { get; set; }
        public string Model { get; set; }
        public string CacheKey { get; set; }
        public Client Client { get; set; }
    }
}
