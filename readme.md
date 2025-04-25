# AI Chat Bot

Integrate your favourite AI chat provider into your community

## Configuration

Host is configured via [appsettings.json](src/AI.Chat.Host.Console/appsettings.json)

Main section of interest is __Chat__

There're 4 subsections:

- Bot
- Moderator
- Adapter
- Client

### Bot

Sample configuration section:

```
    "Bot": {
      "Prompt": "You are a twitch user. Your username is {0}.",
      "Warnings": 3,
      "Apology": "Sorry, something went wrong, please try again.",
      "Filters": [
        {
          "Type": "Regex",
          "Args": [
            "/((https?:\\/\\/)?[^\\s.]+\\.[\\w][^\\s]+)"
          ],
          "Prompt": "Reply should not contain links"
        },
        {
          "Type": "Regex",
          "Args": [
            "/(banword1|banword2)"
          ],
          "Prompt": "These words are prohibited: banword1, banword2"
        },
        {
          "Type": "Length",
          "Args": [
            "400"
          ],
          "Prompt": "Reply should be 400 symbols or shorted."
        }
      ]
    },
```

__Prompt__ is a default system message that describes the context

__Filters__ are used to enforce community rules like prohibiting certain words or technical limitations like message length.
Two types of filters are currently supported:

- Regex
- Length

Filter __Prompt__ is the text that would be sent to the model once reply doesn't meet the expectations

__Warnings__ is the number of times model will be prompted to regenerate reply. If it still fails to do so __Apology__ would be used instead

### Moderator

Sample configuration section:

```
    "Moderator": {
      "Mode": "Public",
      "Moderators": [
        "you"
      ],
      "Promoted": [

      ],
      "Banned": [

      ],
      "Moderated": [

      ]
    },
```

__Mode__ sets default chatting mode. There're two modes:

- Public - all users are allowed to to chat with bot (except __Banned__)
- Restricted - only __Promoted__ users are allowed to chat with bot (except __Banned__)

__Moderators__ are users allowed to execute commands

__Moderated__ are users communication with whom should be reviewed and approved manually by __Moderators__

### Adapter

Sample configuration section:

```
    "Adapter": {
      "Type": "OpenAI",
      "Delay": "00:00:05",
      "ApiKey": "your-api-key",
      "Model": "gemini-2.0-flash",
      "Client": {
        "Endpoint": "https://generativelanguage.googleapis.com/v1beta/openai/",
        "NetworkTimeout": "00:02:00"
      }
    },
```

__Delay__ is a time span between subsequent API requests

As of now following adapters are supported:

- OpenAI - you can use any vendor that is compatible with this API (Google, OpenAI, DeepSeek, Ollama etc)

You must configure __ApiKey__, __Model__ and __Client__ __Endpoint__ with values obrained from your provider (refer to the documentation)

### Clients

Sample configuration section:

```
    "Client": {
      "Username": "bot",

      "Delay": "00:00:10",
      "Prompt": "Message from {0}: {1}",
      "Auth": {
        "Uri": "https://id.twitch.tv/oauth2/",
        "ClientId": "your-client-id",
        "ClientSecret": "your-client-secret",
        "DeviceCode": "",
        "Scopes": [
          "chat:read",
          "chat:edit"
        ]
      },
      "Communication": {
        "MessagesAllowedInPeriod": 750,
        "ThrottlingPeriod": "00:00:30"
      }
    }
```

As of now following clients are supported:

- Twitch

You must create twitch account for bot and register an app (refer to [Twitch Developers](https://dev.twitch.tv/docs/authentication/register-app/)). Once completed, update __Auth__ section with __ClientId__ and __ClientSecret__ obtained from developer console. Upon first run application will generate __DeviceCode__ and fail giving you verification url, for example:

```
Grant access via https://www.twitch.tv/activate?public=true&device-code=ABCDEFGH
```

You must follow given link and activate __DeviceCode__ for the bot account. Alternatively you can set it manually and skip this step (refer to [Device code grant flow](https://dev.twitch.tv/docs/authentication/getting-tokens-oauth/#device-code-grant-flow))

Once started, client will join its' own channel and allow __Moderators__ to execute commands. Supported commands are:

```
!join channel
!leave channel
!ban username1 username2 ...
!unban username1 username2 ...
!timeout username1 hh:mm:ss username2 hh:mm:ss ...
!mod username1 username2 ...
!unmod username1 username2 ...
!promote username1 username2 ...
!demote username1 username2 ...
!mode public
!mode restricted
!allow key1 key2 ...
!allow all
!deny key1 key2 ...
!deny all
!delay hh:mm:ss
!instruct Do not use curse words
!find fromKey toKey
!find all
!get key
!remove key1 key2 ...
!remove all
```

__Join__/__Leave__ will connect/disconnect bot to/from the destination channel.
Connected bot will listen for messages tagged with its' __Username__. Once received, they will be sent to the model using __Prompt__ format

__Ban__/__Unban__, __Mod__/__Unmod__ and __Promote__/__Demote__ will put/remove usernames to/from __Banned__, __Moderated__ and __Promoted__ list correspondingly

Before sending reply to __Moderated__ user it will be sent to bots' own channel, for example:

```
1810210: to be or not to be is a very popular question yet to be answered
```

First part is the message __key__, second one is the actual reply yet to be sent. __Allow__/__Deny__ commands could be used to manage them

You can __Ban__, __Mod__ and __Timeout__ bot itself

__Delay__ will set time span between subsequent replies

__Instruct__ puts a system message for the model

__Find__, __Get__ and __Remove__ could be used to manipulate conversation history

## Extensibility

You can easily introduce your own adapters and clients

Refer to [OpenAI.cs](src/AI.Chat.Adapters.OpenAI/OpenAI.cs) for adapter example, [Twitch.cs](src/AI.Chat.Clients.Twitch/Twitch.cs) for client example and [Program.cs](src/AI.Chat.Host.Console/Program.cs) for DI registration example