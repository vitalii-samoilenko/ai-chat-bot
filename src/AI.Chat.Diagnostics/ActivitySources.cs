namespace AI.Chat.Diagnostics
{
    public class ActivitySources
    {
        public static System.Diagnostics.ActivitySource Adapters = new System.Diagnostics.ActivitySource("AI.Chat.Adapters");
        public static System.Diagnostics.ActivitySource Bots = new System.Diagnostics.ActivitySource("AI.Chat.Bots");
        public static System.Diagnostics.ActivitySource CommandExecutors = new System.Diagnostics.ActivitySource("AI.Chat.CommandExecutors");
        public static System.Diagnostics.ActivitySource Commands = new System.Diagnostics.ActivitySource("AI.Chat.Commands");
        public static System.Diagnostics.ActivitySource Filters = new System.Diagnostics.ActivitySource("AI.Chat.Filters");
        public static System.Diagnostics.ActivitySource Moderators = new System.Diagnostics.ActivitySource("AI.Chat.Moderators");
        public static System.Diagnostics.ActivitySource Scopes = new System.Diagnostics.ActivitySource("AI.Chat.Scopes");
        public static System.Diagnostics.ActivitySource Users = new System.Diagnostics.ActivitySource("AI.Chat.Users");
    }
}
