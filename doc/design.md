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

- null: immediately discards
- memory: caches into collection
- file: persists into text file
- sqlite: persists into database file

## Participants

- Persistent via configuration
- Cannot be constructed

### Console

- Singleton

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

### Solid

![Solid diagram](solid.svg)

### Notification channel

![Notification channel diagram](notification_channel.svg)

### Moderated speech

![Moderated speech diagram](moderated_speech.svg)

### Control interface

![Control interface diagram](control_interface.svg)
