﻿namespace AI.Chat.CommandExecutors.Diagnostics
{
    public class Trace<TCommandExecutor> : ICommandExecutor
        where TCommandExecutor : ICommandExecutor
    {
        private static string CommandExecutorName = $"{typeof(TCommandExecutor).Namespace}.{typeof(TCommandExecutor).Name}";

        private readonly ICommandExecutor _commandExecutor;

        public Trace(TCommandExecutor commandExecutor)
        {
            _commandExecutor = commandExecutor;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string username, string command, string args)
        {
            var activity = AI.Chat.Diagnostics.ActivitySources.CommandExecutors.StartActivity($"{CommandExecutorName}.{nameof(Execute)}");
            System.Collections.Generic.IEnumerator<string> enumerator = null;
            try
            {
                enumerator = _commandExecutor.Execute(username, command, args).GetEnumerator();
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
