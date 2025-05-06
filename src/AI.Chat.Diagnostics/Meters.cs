namespace AI.Chat.Diagnostics
{
    public class Meters
    {
        public static System.Diagnostics.Metrics.Meter Adapters { get; private set; } = new System.Diagnostics.Metrics.Meter("AI.Chat.Adapters");
        public static System.Diagnostics.Metrics.Histogram<int> ReplyLength { get; private set; } = Adapters.CreateHistogram<int>("ai.chat.adapters.reply_length");

        public static System.Diagnostics.Metrics.Meter Scopes { get; private set; } = new System.Diagnostics.Metrics.Meter("AI.Chat.Scopes");
        public static System.Diagnostics.Metrics.Counter<int> ReadOpen { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.read_open");
        public static System.Diagnostics.Metrics.Counter<int> ReadClose { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.read_close");
        public static System.Diagnostics.Metrics.Counter<int> WriteOpen { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.write_open");
        public static System.Diagnostics.Metrics.Counter<int> WriteClose { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.write_close");

        public static System.Diagnostics.Metrics.Meter Users { get; private set; } = new System.Diagnostics.Metrics.Meter("AI.Chat.Users");
        public static System.Diagnostics.Metrics.Histogram<int> MessageLength { get; private set; } = Users.CreateHistogram<int>("ai.chat.users.message_length");
    }
}
