namespace AI.Chat.Options
{
    public enum ModeratorMode
    {
        Public,
        Restricted
    }
    public class Moderator
    {
        public ModeratorMode Mode {  get; set; }
        public System.Collections.Generic.HashSet<string> Moderators { get; set; }
        public System.Collections.Generic.HashSet<string> Promoted { get; set; } = new System.Collections.Generic.HashSet<string>();
        public System.Collections.Generic.HashSet<string> Banned { get; set; } = new System.Collections.Generic.HashSet<string>();
        public System.Collections.Generic.HashSet<string> Moderated { get; set; } = new System.Collections.Generic.HashSet<string>();
    }
}
