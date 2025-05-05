namespace AI.Chat.Diagnostics
{
    public class Counters
    {
        public static System.Diagnostics.Metrics.Meter Scopes { get; private set; } = new System.Diagnostics.Metrics.Meter("AI.Chat.Scopes");

        public static System.Diagnostics.Metrics.Counter<int> ReadOpen { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.read_open");
        public static System.Diagnostics.Metrics.Counter<int> ReadClose { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.read_close");
        public static System.Diagnostics.Metrics.Counter<int> WriteOpen { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.write_open");
        public static System.Diagnostics.Metrics.Counter<int> WriteClose { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.write_close");
    }
}
