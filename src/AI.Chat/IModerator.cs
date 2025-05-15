namespace AI.Chat
{
    public interface IModerator
    {
        bool IsModerator(string username);
        bool IsAllowed(params string[] usernames);
        bool IsModerated(params string[] usernames);
        bool IsWelcomed(params string[] usernames);

        void Ban(params string[] usernames);
        void Unban(params string[] usernames);
        void Timeout(params (string username, System.TimeSpan timeout)[] args);
        void Moderate(params string[] usernames);
        void Unmoderate(params string[] usernames);
        void Promote(params string[] usernames);
        void Demote(params string[] usernames);
        void Welcome(params string[] usernames);
        void Unwelcome(params string[] usernames);

        void Hold(params System.DateTime[] keys);
        void Allow(params System.DateTime[] keys);
        System.Collections.Generic.IEnumerable<System.DateTime> AllowAll();
        void Deny(params System.DateTime[] keys);
        System.Collections.Generic.IEnumerable<System.DateTime> DenyAll();

        bool Greet(string username);
    }
}
