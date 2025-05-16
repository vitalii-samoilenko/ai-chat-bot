namespace AI.Chat
{
    public interface IModerator
    {
        bool IsModerator(string username);
        bool IsAllowed(params string[] usernames);
        bool IsModerated(params string[] usernames);
        bool IsWelcomed(params string[] usernames);

        string[] Ban(params string[] usernames);
        string[] Unban(params string[] usernames);
        (string username, System.DateTime until)[] Timeout(params (string username, System.TimeSpan timeout)[] args);
        string[] Moderate(params string[] usernames);
        string[] Unmoderate(params string[] usernames);
        string[] Promote(params string[] usernames);
        string[] Demote(params string[] usernames);
        string[] Welcome(params string[] usernames);
        string[] Unwelcome(params string[] usernames);

        bool Greet(string username);
    }
}
