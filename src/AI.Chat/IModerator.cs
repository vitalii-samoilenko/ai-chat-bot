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

        void Hold(System.DateTime key, (System.Func<System.Threading.Tasks.Task> onAllowAsync, System.Func<System.Threading.Tasks.Task> onDenyAsync) callbacks);
        System.Func<System.Threading.Tasks.Task> Allow(params System.DateTime[] keys);
        System.Func<System.Threading.Tasks.Task> AllowAll();
        System.Func<System.Threading.Tasks.Task> Deny(params System.DateTime[] keys);
        System.Func<System.Threading.Tasks.Task> DenyAll();

        bool Greet(string username);
    }
}
