namespace AI.Chat
{
    public interface IFilter
    {
        string Prompt { get; }

        bool IsDenied(string message);
    }
}
