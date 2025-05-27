using Microsoft.AspNetCore.Mvc;

namespace AI.Chat.Host.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class CommandsController : ControllerBase
    {
        private readonly AI.Chat.ICommandExecutor _commandExecutor;

        public CommandsController(AI.Chat.ICommandExecutor commandExecutor)
        {
            _commandExecutor = commandExecutor;
        }

        [HttpGet]
        [Route("{command}/execute")]
        public IEnumerable<string> Execute(string username, string command, string args)
        {
            foreach (var token in _commandExecutor.Execute(username, command, args))
            {
                yield return token;
            }
        }
    }
}
