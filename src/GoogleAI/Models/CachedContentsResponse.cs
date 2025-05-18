namespace GoogleAI.Models
{
    public class CachedContentsResponse
    {
        public string Model { get; set; }
        public string Name { get; set; }
        public UsageMetadata UsageMetadata { get; set; }
        public System.DateTime ExpireTime { get; set; }
    }
}
