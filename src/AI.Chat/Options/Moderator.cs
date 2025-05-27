namespace AI.Chat.Options
{
    public class Moderator
    {
        public System.Collections.Generic.HashSet<string> Moderators { get; set; } = new System.Collections.Generic.HashSet<string>(System.StringComparer.OrdinalIgnoreCase);
        public System.Collections.Generic.HashSet<string> Promoted { get; set; } = new System.Collections.Generic.HashSet<string>(System.StringComparer.OrdinalIgnoreCase);
        public System.Collections.Generic.HashSet<string> Banned { get; set; } = new System.Collections.Generic.HashSet<string>(System.StringComparer.OrdinalIgnoreCase);
        public System.Collections.Generic.HashSet<string> Moderated { get; set; } = new System.Collections.Generic.HashSet<string>(System.StringComparer.OrdinalIgnoreCase);
        public System.Collections.Generic.HashSet<string> Welcomed { get; set; } = new System.Collections.Generic.HashSet<string>(System.StringComparer.OrdinalIgnoreCase);
    }
}
