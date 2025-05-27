namespace AI.Chat.Commands.Diagnostics
{
    public class Trace<TCommand> : ICommand
        where TCommand : ICommand
    {
        private static string CommandName = $"{typeof(TCommand).Namespace}.{typeof(TCommand).Name}";

        private readonly ICommand _command;

        public Trace(TCommand command)
        {
            _command = command;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var activity = AI.Chat.Diagnostics.ActivitySources.Commands.StartActivity($"{CommandName}.{nameof(Execute)}");
            System.Collections.Generic.IEnumerator<string> enumerator = null;
            try
            {
                enumerator = _command.Execute(args).GetEnumerator();
                while (enumerator.MoveNext())
                {
                    yield return enumerator.Current;
                }
            }
            finally
            {
                if (activity != null)
                {
                    activity.Dispose();
                }
                if (enumerator != null)
                {
                    enumerator.Dispose();
                }
            }
        }
    }
}
