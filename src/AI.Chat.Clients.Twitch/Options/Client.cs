namespace AI.Chat.Options.Twitch
{
    public class OAuth
    {
        public string Uri { get; set; }
        public string ClientId { get; set; }
        public string ClientSecret { get; set; }
        public string[] Scopes { get; set; }
        public string AccessToken { get; set; }
        public string RefreshToken { get; set; }
        public string DeviceCode { get; set; }
    }
    public enum WelcomeMode
    {
        None,
        OnJoin,
        OnFirstMessage
    }
    public class Welcome
    {
        public WelcomeMode Mode { get; set; }
        public string Prompt { get; set; }
    }
    public class Client
    {
        public string Username { get; set; }
        public string Channel { get; set; }
        public System.TimeSpan Delay { get; set; }
        public string Prompt { get; set; }
        public Welcome Welcome { get; set; }
        public OAuth Auth { get; set; }
        public TwitchLib.Communication.Models.ClientOptions Communication { get; set; }
    }
}