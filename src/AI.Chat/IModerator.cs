namespace AI.Chat
{
    public interface IModerator
    {
        bool IsModerator(string username);
        bool IsAllowed(System.Collections.Generic.IEnumerable<string> usernames);
        bool IsModerated(System.Collections.Generic.IEnumerable<string> usernames);
        bool IsWelcomed(System.Collections.Generic.IEnumerable<string> usernames);

        System.Collections.Generic.List<string> Ban(System.Collections.Generic.IEnumerable<string> usernames);
        System.Collections.Generic.List<string> Unban(System.Collections.Generic.IEnumerable<string> usernames);
        System.Collections.Generic.List<(string username, System.DateTime until)> Timeout(System.Collections.Generic.IEnumerable<(string username, System.TimeSpan timeout)> args);
        System.Collections.Generic.List<string> Moderate(System.Collections.Generic.IEnumerable<string> usernames);
        System.Collections.Generic.List<string> Unmoderate(System.Collections.Generic.IEnumerable<string> usernames);
        System.Collections.Generic.List<string> Promote(System.Collections.Generic.IEnumerable<string> usernames);
        System.Collections.Generic.List<string> Demote(System.Collections.Generic.IEnumerable<string> usernames);
        System.Collections.Generic.List<string> Welcome(System.Collections.Generic.IEnumerable<string> usernames);
        System.Collections.Generic.List<string> Unwelcome(System.Collections.Generic.IEnumerable<string> usernames);

        bool Greet(string username);
    }
}
