using Microsoft.AspNetCore.Mvc;

namespace AI.Chat.Hosts.API.Moderator.Server.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class CommandsController : ControllerBase
    {
        private readonly IClient _client;

        public CommandsController(IClient client)
        {
            _client = client;
        }

        [HttpGet]
        [Route("{command}/execute")]
        public async Task<IEnumerable<string>> ExecuteAsync(string command, string args)
        {
            var tokens = await _client.ExecuteAsync(string.Empty, command, args);
            return tokens;
        }
    }
}
