namespace GoogleAI.Diagnostics
{
    public static class ActivitySources
    {
        public static System.Diagnostics.ActivitySource Client { get; private set; } = new System.Diagnostics.ActivitySource("GoogleAI.Client");
    }
}
