# Design

![Package diagram](package.svg)

## Structure

![Structure diagram](structure.svg)

## Threads

Message repository

- Persistent via configuration
- Cannot be constructed

### Backed

![Backed thread diagram](threads_backed.svg)

- sqlite: persists into database file
- memory: caches into collection
- null: immediately discards

## Participants

- Persistent via configuration
- Cannot be constructed

### Console

### OpenAI

### Moderator

### Twitch

## Repositories

![Repository diagram](repository.svg)

## Logical constructs

Blocks and communication patterns

### Pipe

![Pipe diagram](pipe.svg)

### Cache

![Cache diagram](cache.svg)

### Notification channel

![Notification channel diagram](notification_channel.svg)

### Moderated channel

![Moderated channel diagram](moderated_channel.svg)
