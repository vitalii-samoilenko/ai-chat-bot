namespace AI.Chat
{
    public interface IFilter
    {
        bool IsDenied(string message, out string reason);
    }
}
