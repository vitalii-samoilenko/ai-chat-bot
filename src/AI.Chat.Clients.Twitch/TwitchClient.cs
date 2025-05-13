using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using TwitchLib.Client.Events;
using TwitchLib.Client.Interfaces;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Events;

namespace TwitchLib.Client
{
    public class DummyTwitchClient : ITwitchClient
    {
        private readonly ILogger<DummyTwitchClient> _logger;
        private readonly List<JoinedChannel> _joinedChannels;
        private Task _client;

        public DummyTwitchClient(
            ILogger<DummyTwitchClient> logger)
        {
            _logger = logger;
            _joinedChannels = new List<JoinedChannel>();
        }

        public bool AutoReListenOnException { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public MessageEmoteCollection ChannelEmotes => throw new NotImplementedException();

        public ConnectionCredentials ConnectionCredentials => throw new NotImplementedException();

        public bool DisableAutoPong { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public bool IsConnected => throw new NotImplementedException();

        public bool IsInitialized => throw new NotImplementedException();

        public IReadOnlyList<JoinedChannel> JoinedChannels => _joinedChannels;

        public WhisperMessage PreviousWhisper => throw new NotImplementedException();

        public string TwitchUsername => throw new NotImplementedException();

        public bool WillReplaceEmotes { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public event EventHandler<OnChannelStateChangedArgs> OnChannelStateChanged;
        public event EventHandler<OnChatClearedArgs> OnChatCleared;
        public event EventHandler<OnChatColorChangedArgs> OnChatColorChanged;
        public event EventHandler<OnChatCommandReceivedArgs> OnChatCommandReceived;
        public event EventHandler<OnConnectedArgs> OnConnected;
        public event EventHandler<OnConnectionErrorArgs> OnConnectionError;
        public event EventHandler<OnDisconnectedEventArgs> OnDisconnected;
        public event EventHandler<OnExistingUsersDetectedArgs> OnExistingUsersDetected;
        public event EventHandler<OnGiftedSubscriptionArgs> OnGiftedSubscription;
        public event EventHandler<OnIncorrectLoginArgs> OnIncorrectLogin;
        public event EventHandler<OnJoinedChannelArgs> OnJoinedChannel;
        public event EventHandler<OnLeftChannelArgs> OnLeftChannel;
        public event EventHandler<OnLogArgs> OnLog;
        public event EventHandler<OnMessageReceivedArgs> OnMessageReceived;
        public event EventHandler<OnMessageSentArgs> OnMessageSent;
        public event EventHandler<OnModeratorJoinedArgs> OnModeratorJoined;
        public event EventHandler<OnModeratorLeftArgs> OnModeratorLeft;
        public event EventHandler<OnModeratorsReceivedArgs> OnModeratorsReceived;
        public event EventHandler<OnNewSubscriberArgs> OnNewSubscriber;
        public event EventHandler<OnRaidNotificationArgs> OnRaidNotification;
        public event EventHandler<OnReSubscriberArgs> OnReSubscriber;
        public event EventHandler<OnSendReceiveDataArgs> OnSendReceiveData;
        public event EventHandler<OnUserBannedArgs> OnUserBanned;
        public event EventHandler<OnUserJoinedArgs> OnUserJoined;
        public event EventHandler<OnUserLeftArgs> OnUserLeft;
        public event EventHandler<OnUserStateChangedArgs> OnUserStateChanged;
        public event EventHandler<OnUserTimedoutArgs> OnUserTimedout;
        public event EventHandler<OnWhisperCommandReceivedArgs> OnWhisperCommandReceived;
        public event EventHandler<OnWhisperReceivedArgs> OnWhisperReceived;
        public event EventHandler<OnWhisperSentArgs> OnWhisperSent;
        public event EventHandler<OnMessageThrottledEventArgs> OnMessageThrottled;
        public event EventHandler<OnWhisperThrottledEventArgs> OnWhisperThrottled;
        public event EventHandler<OnErrorEventArgs> OnError;
        public event EventHandler<OnReconnectedEventArgs> OnReconnected;
        public event EventHandler<OnVIPsReceivedArgs> OnVIPsReceived;
        public event EventHandler<OnCommunitySubscriptionArgs> OnCommunitySubscription;
        public event EventHandler<OnMessageClearedArgs> OnMessageCleared;
        public event EventHandler<OnRequiresVerifiedEmailArgs> OnRequiresVerifiedEmail;
        public event EventHandler<OnRequiresVerifiedPhoneNumberArgs> OnRequiresVerifiedPhoneNumber;
        public event EventHandler<OnBannedEmailAliasArgs> OnBannedEmailAlias;
        public event EventHandler<OnUserIntroArgs> OnUserIntro;
        public event EventHandler<OnAnnouncementArgs> OnAnnouncement;

        public void AddChatCommandIdentifier(char identifier)
        {
            throw new NotImplementedException();
        }

        public void AddWhisperCommandIdentifier(char identifier)
        {
            throw new NotImplementedException();
        }

        public bool Connect()
        {
            _logger.LogInformation("Connecting...");

            if (_client != null)
            {
                return true;
            }
            _client = Task.Run(() =>
            {
                while (true)
                {
                    var message = Console.ReadLine();
                    if (!message.StartsWith("~"))
                    {
                        continue;
                    }
                    var delimiter = message.IndexOf('~', 1);
                    var chatMessage = new ChatMessage(
                        null, null, message.Substring(1, delimiter - 1), null, null,
                        System.Drawing.Color.Empty, null, message.Substring(delimiter + 1).Trim(), Enums.UserType.Viewer, null, Guid.NewGuid().ToString(),
                        false, 0, null, false, false, false,
                        false, false, false, false, Enums.Noisy.False, null,
                        null, null, null, 0, 0);
                    OnMessageReceived(
                        this,
                        new OnMessageReceivedArgs
                        {
                            ChatMessage = chatMessage
                        });
                    if (chatMessage.Message.StartsWith("!"))
                    {
                        OnChatCommandReceived(
                            this,
                            new OnChatCommandReceivedArgs
                            {
                                Command = new ChatCommand(chatMessage)
                            });
                    }
                }
            });

            return true;
        }

        public void Disconnect()
        {
            throw new NotImplementedException();
        }

        public JoinedChannel GetJoinedChannel(string channel)
        {
            throw new NotImplementedException();
        }

        public void Initialize(ConnectionCredentials credentials, string channel = null, char chatCommandIdentifier = '!', char whisperCommandIdentifier = '!', bool autoReListenOnExceptions = true)
        {
            _logger.LogInformation($"Initializing channel {channel}...");
            _joinedChannels.Add(new JoinedChannel(channel));
        }

        public void Initialize(ConnectionCredentials credentials, List<string> channels, char chatCommandIdentifier = '!', char whisperCommandIdentifier = '!', bool autoReListenOnExceptions = true)
        {
            throw new NotImplementedException();
        }

        public void JoinChannel(string channel, bool overrideCheck = false)
        {
            throw new NotImplementedException();
        }

        public void LeaveChannel(JoinedChannel channel)
        {
            throw new NotImplementedException();
        }

        public void LeaveChannel(string channel)
        {
            throw new NotImplementedException();
        }

        public void OnReadLineTest(string rawIrc)
        {
            throw new NotImplementedException();
        }

        public void Reconnect()
        {
            throw new NotImplementedException();
        }

        public void RemoveChatCommandIdentifier(char identifier)
        {
            throw new NotImplementedException();
        }

        public void RemoveWhisperCommandIdentifier(char identifier)
        {
            throw new NotImplementedException();
        }

        public void SendMessage(JoinedChannel channel, string message, bool dryRun = false)
        {
            _logger.LogInformation($"#{channel.Channel}: {message}");
        }

        public void SendMessage(string channel, string message, bool dryRun = false)
        {
            _logger.LogInformation($"#{channel}: {message}");
        }

        public void SendQueuedItem(string message)
        {
            throw new NotImplementedException();
        }

        public void SendRaw(string message)
        {
            throw new NotImplementedException();
        }

        public void SendReply(JoinedChannel channel, string replyToId, string message, bool dryRun = false)
        {
            throw new NotImplementedException();
        }

        public void SendReply(string channel, string replyToId, string message, bool dryRun = false)
        {
            _logger.LogInformation($"#{channel} ({replyToId}): {message}");
        }

        public void SendWhisper(string receiver, string message, bool dryRun = false)
        {
            throw new NotImplementedException();
        }

        public void SetConnectionCredentials(ConnectionCredentials credentials)
        {
            throw new NotImplementedException();
        }
    }
}
