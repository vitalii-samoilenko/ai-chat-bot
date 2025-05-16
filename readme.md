# AI Chat Bot

Integrate your favourite AI chat provider into your community

## Configuration

Host is configured via [appsettings.json](src/AI.Chat.Hosts.Console/appsettings.json)

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
      "Warnings": 3,
      "Apology": "{0} sorry, something went wrong, please try again.",
      "Filters": [
        {
          "Type": "Regex",
          "Args": [
            "/((https?:\\/\\/)?[^\\s.]+\\.[\\w][^\\s]+)"
          ],
          "Reason": "Reply should not contain links"
        },
        {
          "Type": "Regex",
          "Args": [
            "/(banword1|banword2)"
          ],
          "Reason": "These words are prohibited: banword1, banword2"
        },
        {
          "Type": "Length",
          "Args": [
            "400"
          ],
          "Reason": "Reply should be 400 symbols or shorted."
        }
      ]
    }
```

__Filters__ are used to enforce community rules like prohibiting certain words or technical limitations like message length.
Two types of filters are currently supported:

- Regex
- Length

Filter __Reason__ is the text that would be sent to the model once reply doesn't meet the expectations

__Warnings__ is the number of times model will be prompted to regenerate reply. If it still fails to do so __Apology__ would be used instead

### Moderator

Sample configuration section:

```
    "Moderator": {
      "Moderators": [
        "you"
      ],
      "Promoted": [
        "botusername"
      ],
      "Banned": [

      ],
      "Moderated": [

      ],
      "Welcomed": [

      ]
    }
```

__Promoted__ are users allowed to chat with bot (except __Banned__)

__Moderators__ are users allowed to execute commands

__Moderated__ are users communication with whom should be reviewed and approved manually by __Moderators__

__Welcomed__ are users who will be greeted if supported by client

### Adapter

Sample configuration section:

```
    "Adapter": {
      "Type": "OpenAI",
      "Delay": "00:00:05",
      "Threshold": 82000,
      "Skip": 5,
      "Period": "24:00:00",
      "ApiKey": "your-api-key",
      "Model": "gemini-2.0-flash",
      "Client": {
        "Endpoint": "https://generativelanguage.googleapis.com/v1beta/openai/",
        "NetworkTimeout": "00:02:00"
      }
    }
```

__Delay__ is a time span between subsequent API requests

__Threshold__ is a number of tokens exceeding which would trigger history cleanup process

__Skip__ is a number of initial history records (i.e. context description, communication examples) to keep once cleanup is triggered

__Period__ is a time span to "forget" during history cleanup

As of now following adapter __Types__ are supported:

- OpenAI - you can use any vendor that is compatible with this API (Google, OpenAI, DeepSeek, Ollama etc)

You must configure __ApiKey__, __Model__ and __Client__ __Endpoint__ with values obrained from your provider (refer to the documentation)

### Clients

Sample configuration section:

```
    "Client": {
      "Type": "Twitch",
      "Username": "botusername",
      "Joined": "{0} has joined chat",
      "Prompt": "{0}: {1}",

      "Delay": "00:00:10",
      "Welcome": {
        "Mode": "OnFirstMessage"
      },
      "Auth": {
        "Uri": "https://id.twitch.tv/oauth2/",
        "ClientId": "your-client-id",
        "ClientSecret": "your-client-secret",
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

As of now following client __Types__ are supported:

- Twitch

You must create twitch account for bot and register an applicaion (refer to [Twitch Developers](https://dev.twitch.tv/docs/authentication/register-app/)). Once completed, update __Auth__ section with __ClientId__ and __ClientSecret__ obtained from developer console. Upon first run application will generate __DeviceCode__ and fail giving you verification url, for example:

```
Grant access via https://www.twitch.tv/activate?public=true&device-code=ABCDEFGH
```

You must follow given link and activate __DeviceCode__ for the bot account.

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
!welcome username1 username2 ...
!unwelcome username1 username2 ...
!cheerful none
!cheerful onjoin
!cheerful onfirstmessage
!allow key1 key2 ...
!allow all
!delay hh:mm:ss
!instruct Do not use curse words
!find fromKey toKey
!find fromKey toKey tag1 tag2 ...
!find all
!find all tag1 tag2 ...
!get key
!remove key1 key2 ...
!remove all
!edit key message
```

__Join__/__Leave__ will connect/disconnect bot to/from the destination channel.
Connected bot will listen for messages tagged with its' __Username__. Once received, they will be sent to the model using __Prompt__ format

__Ban__/__Unban__, __Mod__/__Unmod__, __Welcome__/__Unwelcome__ and __Promote__/__Demote__ will add/remove usernames to/from __Banned__, __Moderated__, __Welcomed__ and __Promoted__ list correspondingly

Before sending reply to __Moderated__ user it will be sent to bots' own channel, as well as original question, for example:

```
202505102244507310000 to be or not to be is a very popular question yet to be answered
```

First part is the message __key__, second one is the actual reply yet to be sent. __Allow__/__Remove__ commands could be used to manage them

You can __Ban__, __Mod__, __Welcome__, __Promote__ and __Timeout__ bot itself:
 - __Ban__ and __Timeout__ will mute bot
 - __Mod__ will moderate every bots' reply
 - __Welcome__ will greet everyone
 - __Promote__ will allow everyone to chat with bot

__Cheerful__ sets greeting mode

__Delay__ will set time span between subsequent replies

__Instruct__ puts a system message for the model

__Find__, __Get__, __Remove__ and __Edit__ could be used to manipulate conversation history

## History

Host will persist conversation history on disk in __external/history.log__ file

Sample history:

```
202505102244507310000
{"Key":"202505102244507310001","Value":{"Message":"You are a twitch user. Your username is botusername.","Tags":["type=system"]}}
{"Key":"202505102244507310002","Value":{"Message":"some_user: @botusername how is it going?","Tags":["type=user","user.name=some_user"]}}
{"Key":"202505102244507310003","Value":{"Message":"@some_user good, thank you!","Tags":["type=model"]}}
{"Key":"202505102244507310004","Value":{"Message":"other_user has joined chat","Tags":["type=user","user.name=other_user"]}}
{"Key":"202505102244507310005","Value":{"Message":"Welcome @other_user, have a great time!","Tags":["type=model"]}}
```

It is recommended to manually craft context and conversation examples using __Client__ __Joined__ and __Prompt__ formats for the model to follow them and exclude these records from cleanup process with __Adapter__ __Skip__ setting

## Extensibility

You can easily introduce your own adapters and clients

Refer to [OpenAI.cs](src/AI.Chat.Adapters.OpenAI/OpenAI.cs) for adapter example, [Twitch.cs](src/AI.Chat.Clients.Twitch/Twitch.cs) for client example and [ServiceCollection.cs](src/AI.Chat.Host/Extensions/ServiceCollection.cs) for DI registration example