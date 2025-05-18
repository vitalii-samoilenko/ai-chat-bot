namespace GoogleAI.Models
{
    public class CachedContentsRequest
    {
        public string Model { get; set; }
        public Content[] Contents { get; set; }
        public Content SystemInstruction { get; set; }
        public string Ttl { get; set; }
    }
}
