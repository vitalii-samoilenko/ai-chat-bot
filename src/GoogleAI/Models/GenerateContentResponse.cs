namespace GoogleAI.Models
{
    public class GenerateContentResponse
    {
        public System.Collections.Generic.List<Candidate> Candidates { get; set; }
        public UsageMetadata UsageMetadata { get; set; }
    }
}
