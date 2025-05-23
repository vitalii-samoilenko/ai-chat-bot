﻿using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using TwitchLib.Client.Events;
using TwitchLib.Client.Interfaces;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Events;

namespace TwitchLib.Client.Diagnostics
{
    public class ExceptionHandler<TTwitchClient> : ITwitchClient
        where TTwitchClient : ITwitchClient
    {
        private readonly ITwitchClient _client;
        private readonly ILogger<ExceptionHandler<TTwitchClient>> _logger;

        public ExceptionHandler(TTwitchClient client, ILogger<ExceptionHandler<TTwitchClient>> logger)
        {
            _client = client;
            _logger = logger;
        }

        public bool AutoReListenOnException { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public MessageEmoteCollection ChannelEmotes => throw new NotImplementedException();

        public ConnectionCredentials ConnectionCredentials => throw new NotImplementedException();

        public bool DisableAutoPong { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public bool IsConnected => throw new NotImplementedException();

        public bool IsInitialized => throw new NotImplementedException();

        public IReadOnlyList<JoinedChannel> JoinedChannels => _client.JoinedChannels;

        public WhisperMessage PreviousWhisper => throw new NotImplementedException();

        public string TwitchUsername => throw new NotImplementedException();

        public bool WillReplaceEmotes { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public event EventHandler<OnChannelStateChangedArgs> OnChannelStateChanged;
        public event EventHandler<OnChatClearedArgs> OnChatCleared;
        public event EventHandler<OnChatColorChangedArgs> OnChatColorChanged;
        public event EventHandler<OnChatCommandReceivedArgs> OnChatCommandReceived
        {
            add
            {
                _client.OnChatCommandReceived += (sender, args) =>
                {
                    try
                    {
                        value(sender, args);
                    }
                    catch (Exception e)
                    {
                        _logger.LogError(e, nameof(OnChatCommandReceived));
                    }
                };
            }
            remove
            {

            }
        }
        public event EventHandler<OnConnectedArgs> OnConnected;
        public event EventHandler<OnConnectionErrorArgs> OnConnectionError;
        public event EventHandler<OnDisconnectedEventArgs> OnDisconnected;
        public event EventHandler<OnExistingUsersDetectedArgs> OnExistingUsersDetected;
        public event EventHandler<OnGiftedSubscriptionArgs> OnGiftedSubscription;
        public event EventHandler<OnIncorrectLoginArgs> OnIncorrectLogin;
        public event EventHandler<OnJoinedChannelArgs> OnJoinedChannel;
        public event EventHandler<OnLeftChannelArgs> OnLeftChannel;
        public event EventHandler<OnLogArgs> OnLog;
        public event EventHandler<OnMessageReceivedArgs> OnMessageReceived
        {
            add
            {
                _client.OnMessageReceived += (sender, args) =>
                {
                    try
                    {
                        value(sender, args);
                    }
                    catch (Exception e)
                    {
                        _logger.LogError(e, nameof(OnMessageReceived));
                    }
                };
            }
            remove
            {

            }
        }
        public event EventHandler<OnMessageSentArgs> OnMessageSent;
        public event EventHandler<OnModeratorJoinedArgs> OnModeratorJoined;
        public event EventHandler<OnModeratorLeftArgs> OnModeratorLeft;
        public event EventHandler<OnModeratorsReceivedArgs> OnModeratorsReceived;
        public event EventHandler<OnNewSubscriberArgs> OnNewSubscriber;
        public event EventHandler<OnRaidNotificationArgs> OnRaidNotification;
        public event EventHandler<OnReSubscriberArgs> OnReSubscriber;
        public event EventHandler<OnSendReceiveDataArgs> OnSendReceiveData;
        public event EventHandler<OnUserBannedArgs> OnUserBanned;
        public event EventHandler<OnUserJoinedArgs> OnUserJoined
        {
            add
            {
                _client.OnUserJoined += (sender, args) =>
                {
                    try
                    {
                        value(sender, args);
                    }
                    catch (Exception e)
                    {
                        _logger.LogError(e, nameof(OnUserJoined));
                    }
                };
            }
            remove
            {

            }
        }
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
            return _client.Connect();
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
            _client.Initialize(credentials, channel, chatCommandIdentifier, whisperCommandIdentifier, autoReListenOnExceptions);
        }

        public void Initialize(ConnectionCredentials credentials, List<string> channels, char chatCommandIdentifier = '!', char whisperCommandIdentifier = '!', bool autoReListenOnExceptions = true)
        {
            _client.Initialize(credentials, channels, chatCommandIdentifier, whisperCommandIdentifier, autoReListenOnExceptions);
        }

        public void JoinChannel(string channel, bool overrideCheck = false)
        {
            _client.JoinChannel(channel, overrideCheck);
        }

        public void LeaveChannel(JoinedChannel channel)
        {
            _client.LeaveChannel(channel);
        }

        public void LeaveChannel(string channel)
        {
            _client.LeaveChannel(channel);
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
            _client.SendMessage(channel, message, dryRun);
        }

        public void SendMessage(string channel, string message, bool dryRun = false)
        {
            _client.SendMessage(channel, message, dryRun);
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
            _client.SendReply(channel, replyToId, message, dryRun);
        }

        public void SendReply(string channel, string replyToId, string message, bool dryRun = false)
        {
            _client.SendReply(channel, replyToId, message, dryRun);
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
