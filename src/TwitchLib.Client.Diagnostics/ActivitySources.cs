namespace TwitchLib.Client.Diagnostics
{
    public static class ActivitySources
    {
        public static System.Diagnostics.ActivitySource Client { get; private set; } = new System.Diagnostics.ActivitySource("TwitchLib.Client");
    }
}
