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

## Logical constructs

Communication patterns

### Notification channel

![Notification channel diagram](notification_channel.svg)

### Moderated channel

![Moderated channel diagram](moderated_channel.svg)
