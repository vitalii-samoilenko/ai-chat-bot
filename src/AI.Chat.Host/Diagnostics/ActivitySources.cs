namespace AI.Chat.Host.Diagnostics
{
    public static class ActivitySources
    {
        public static System.Diagnostics.ActivitySource Services { get; private set; } = new System.Diagnostics.ActivitySource("AI.Chat.Host.Services");
    }
}
