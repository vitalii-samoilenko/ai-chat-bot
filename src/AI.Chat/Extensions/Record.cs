﻿namespace AI.Chat.Extensions
{
    public static class Record
    {
        public static bool IsModerated(this AI.Chat.Record record)
        {
            foreach (var tag in record.Tags)
            {
                if (Constants.TagModerated.Equals(tag, System.StringComparison.OrdinalIgnoreCase))
                {
                    return true;
                }
            }
            return false;
        }
        public static bool IsSystemInstruction(this AI.Chat.Record record)
        {
            foreach (var tag in record.Tags)
            {
                if ((Constants.TagType + "=" + Constants.TypeSystem).Equals(tag, System.StringComparison.OrdinalIgnoreCase))
                {
                    return true;
                }
            }
            return false;
        }
    }
}
