using Microsoft.AspNetCore.Mvc;

namespace AI.Chat.Host.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class CommandsController : ControllerBase
    {
        private readonly ILogger<CommandsController> _logger;
        private readonly AI.Chat.ICommandExecutor _commandExecutor;

        public CommandsController(ILogger<CommandsController> logger, AI.Chat.ICommandExecutor commandExecutor)
        {
            _logger = logger;
            _commandExecutor = commandExecutor;
        }

        [HttpGet]
        [Route("{command}/execute")]
        public async Task<bool> ExecuteAsync(string command, string args)
        {
            _logger.LogInformation($"!{command} {args}");

            return await _commandExecutor.ExecuteAsync(command, args);
        }
    }
}
