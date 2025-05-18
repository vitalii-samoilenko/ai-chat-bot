namespace GoogleAI.Models
{
    public class GenerateContentRequest
    {
        public Content[] Contents { get; set; }
        public Content SystemInstruction { get; set; }
        public string CachedContent { get; set; }
    }
}
