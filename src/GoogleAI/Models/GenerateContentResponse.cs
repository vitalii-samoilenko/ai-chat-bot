namespace GoogleAI.Models
{
    public class GenerateContentResponse
    {
        public Candidate[] Candidates { get; set; }
        public UsageMetadata UsageMetadata { get; set; }
    }
}
