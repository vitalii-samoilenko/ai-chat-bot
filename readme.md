# AI Chat Bot

Integrate your favourite AI chat provider into your community

## Limitations

> [!CAUTION]
> SECURITY ISSUE! Although software will use secure TLS protocol to communicate with endpoints certificate validation IS DISABLED

 - Incorrect time range calculation for timezones with negative offsets
 - Only HTTPS/WSS protocol support
 - Bugs (by using this software you are doing so at your own risk)

## Configuration

Host is configured via config.json

Sample configuration file:

```
{
	"botname": "botname",
	"administrators": [
		"username"
	],
	"allowed": [
		"botname"
	],
	"filters": [
		{
			"name": "length",
			"pattern": ".{200}"
		},
		{
			"name": "links",
			"pattern": "(?:http(?:s)?:\/\/)?(?:www\\.)?\\w+\\.\\w+"
		},
		{
			"name": "banwords",
			"pattern": "(?i)some|really|bad|words"
		}
	],
	"retries": 3,
	"apology": "@{username} sorry, something went wrong...",
	"pattern": "{username}: {content}",
	"context": [
		{
			"tags": [
				{
					"name": "source",
					"value": "config"
				},
				{
					"name": "system",
					"value": ""
				}
			],
			"content": "You are a twitch user. Your username is botusername."
		},
		{
			"tags": [
				{
					"name": "source",
					"value": "config"
				},
				{
					"name": "user.name",
					"value": "johndoe"
				}
			],
			"content": "@botusername how is it going?"
		},
		{
			"tags": [
				{
					"name": "source",
					"value": "config"
				},
				{
					"name": "user.name",
					"value": "botname"
				}
			],
			"content": "@johndoe good, thank you!"
		},
		{
			"tags": [
				{
					"name": "source",
					"value": "config"
				},
				{
					"name": "user.name",
					"value": "janeparker"
				}
			],
			"content": "@botname let's have fun today!"
		},
		{
			"tags": [
				{
					"name": "source",
					"value": "config"
				},
				{
					"name": "user.name",
					"value": "botname"
				}
			],
			"content": "@janeparker sure, lets rock!"
		}
	],
	"client": {
		"type": "twitch",
		"auth": {
			"address": "https://id.twitch.tv/oauth2/",
			"timeout": 30000,
			"client_id": "client_id",
			"client_secret": "client_secret",
			"device_code": "device_code",
			"refresh_token": "refresh_token",
			"access_token": "access_token"
		},
		"address": "wss://irc-ws.chat.twitch.tv/",
		"timeout": 30000,
		"delay": 3000
	},
	"history": {
		"type": "sqlite",
		"filename": "chat"
	},
	"adapter": {
		"type": "openai",
		"address": "https://generativelanguage.googleapis.com/v1beta/openai/",
		"timeout": 30000,
		"delay": 30000,
		"limit": 999000,
		"model": "gemini-2.0-flash",
		"key": "key",
		"skip": 1,
		"range": 1
	},
	"moderator": {
		"type": "sqlite",
		"filename": "controller"
	}
}
```

### Common

__administrators__ are users allowed to execute commands

__allowed__ are users allowed to chat with bot

__filters__ are regular expressions used to enforce community rules like prohibiting certain words or posting links. Detailed syntax documentation could be found here: [RE2 Syntax Wiki](https://github.com/google/re2/wiki/Syntax)

__pattern__ is used to transform input message into model representation. Supported placeholders are: {username} and {content}

__retries__ is the number of times model will be prompted to regenerate reply. If it still fails to do so formatted __apology__ would be used instead

__context__ is the initial data that would be put into history. It is recommended to manually craft context and conversation examples for the model to follow it and exclude these records from cleanup process with __adapter__ __skip__ setting

### History

As of now following history __types__ are supported:

- SQLite

__filename__ specifies where context data would be stored

### Moderator

As of now following moderator __types__ are supported:

- SQLite

__filename__ specifies where configuration data would be stored

### Adapter

As of now following adapter __types__ are supported:

- OpenAI - you can use any vendor that is compatible with this API (Google, OpenAI, DeepSeek, Ollama etc)

You must configure __model__, __key__ and __address__ with values obrained from your provider (refer to the documentation)

__delay__ is a time span between subsequent API requests

__limit__ is a number of tokens exceeding which would trigger history cleanup process

__skip__ is a number of initial history records (i.e. context description, communication examples) to keep once cleanup is triggered

__range__ is a time span to "forget" during history cleanup measured in days

### Clients

As of now following client __types__ are supported:

- Twitch

You must create twitch account for bot and register an applicaion (refer to [Twitch Developers](https://dev.twitch.tv/docs/authentication/register-app/)). Also you must manually activate your device, obtain __refresh_token__ and update __auth__ section (refer to [Device code grant flow](https://dev.twitch.tv/docs/authentication/getting-tokens-oauth/#device-code-grant-flow))

Once started, client will join its' own channel and allow __administrators__ to execute commands. Supported commands are:

```
!join channel
!leave
!ban username
!unban username
!timeout username hh:mm:ss
!mod username
!unmod username
!allow username
!deny username
!instruct Do not use curse words
!find YYYY-MM-DDThh:mm:ss YYYY-MM-DDThh:mm:ss
!find YYYY-MM-DDThh:mm:ss YYYY-MM-DDThh:mm:ss name1=value1 name2=value2 ...
!content key
!remove key
!remove YYYY-MM-DDThh:mm:ss YYYY-MM-DDThh:mm:ss
!edit key message
```

__join__/__leave__ will connect/disconnect bot to/from the destination channel. Only one channel at a time is supported. Connected bot will listen for messages tagged with its' __botname__. Once received, they will be sent to the model using __pattern__ format

__mod__/__unmod__ will grant/revoke privileges to execute commands (__administrators__ are permanent moderators)

You can __ban__, __allow__ and __timeout__ bot itself:
 - __ban__ and __timeout__ will mute bot
 - __allow__ will allow everyone to chat with bot

__instruct__ puts a system message for the model

__find__, __content__, __remove__ and __edit__ could be used to manipulate conversation history

## Extensibility

You can easily introduce your own adapters and clients

Refer to [openai.hpp](src/adapters/openai/include/ai/chat/adapters/openai.hpp) for adapter example, [twitch.hpp](src/clients/twitch/include/ai/chat/clients/twitch.hpp) for client example and [openai.hpp](src/binders/openai/include/ai/chat/binders/openai.hpp) for binder example