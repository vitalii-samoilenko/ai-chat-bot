using AI.Chat.Extensions;

namespace AI.Chat.Clients
{
    public class Console
    {
        private readonly ICommandExecutor _commandExecutor;
        private System.Threading.Tasks.Task _worker;

        public Console(ICommandExecutor commandExecutor)
        {
            _commandExecutor = commandExecutor;
        }

        public void Start()
        {
            _worker = System.Threading.Tasks.Task.Run(() =>
            {
                for (;;)
                {
                    var line = System.Console.ReadLine();
                    if (string.IsNullOrWhiteSpace(line)
                        || !line.StartsWith("!", System.StringComparison.OrdinalIgnoreCase))
                    {
                        continue;
                    }
                    try
                    {
                        foreach (var token in _commandExecutor.Execute(line.ExtractToken(out line).Substring(1), line))
                        {
                            System.Console.WriteLine(token);
                        }
                    }
                    catch (System.Exception ex)
                    {
                        System.Console.WriteLine(ex);
                    }
                }
            });
        }
    }
}
