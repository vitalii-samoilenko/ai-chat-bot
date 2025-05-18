namespace AI.Chat.Diagnostics
{
    public static class ActivitySources
    {
        public static System.Diagnostics.ActivitySource Adapters { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Adapters");
        public static System.Diagnostics.ActivitySource Bots { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Bots");
        public static System.Diagnostics.ActivitySource Clients { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Clients");
        public static System.Diagnostics.ActivitySource CommandExecutors { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.CommandExecutors");
        public static System.Diagnostics.ActivitySource Commands { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Commands");
        public static System.Diagnostics.ActivitySource Filters { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Filters");
        public static System.Diagnostics.ActivitySource Histories { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Histories");
        public static System.Diagnostics.ActivitySource Moderators { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Moderators");
        public static System.Diagnostics.ActivitySource Scopes { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Scopes");
    }
}
