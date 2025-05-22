namespace GoogleAI.Models
{
    public class GenerateContentRequest
    {
        public System.Collections.Generic.List<Content> Contents { get; set; }
        public Content SystemInstruction { get; set; }
        public string CachedContent { get; set; }
        public GenerationConfig GenerationConfig { get; set; }
    }
}
