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
        public async Task<bool> ExecuteAsync(string command, string args)
        {
            return await _commandExecutor.ExecuteAsync(command, args);
        }
    }
}
