namespace AI.Chat.Adapters.Extensions.GoogleAI
{
    public static class Record
    {
        public static global::GoogleAI.Models.Content ToContent(this AI.Chat.Record record)
        {
            foreach (var tag in record.Tags)
            {
                if (!tag.StartsWith(Defaults.TagType + "=", System.StringComparison.OrdinalIgnoreCase))
                {
                    continue;
                }
                return new global::GoogleAI.Models.Content
                {
                    Role = tag.Substring(Defaults.TagType.Length + 1),
                    Parts = new System.Collections.Generic.List<global::GoogleAI.Models.Part>
                    {
                        new global::GoogleAI.Models.Part
                        {
                            Text = record.Message
                        }
                    }
                };
            }
            throw new System.ArgumentException("Record is invalid", nameof(record));
        }
    }
}
