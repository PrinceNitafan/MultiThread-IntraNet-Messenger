#ifndef _RECEIVER_SENDER_H_
#define _RECEIVER_SENDER_H_

#include "list.h"

// for startup
void receive_send_init(char* recPort, char* remote_address, char* senPort); 
void receive_sendShutdown(void);

// for receiving
void receiverInputNewMessageToReceiveList(List* plist, void* receivedMessage);
void receiverGetNewMessage(char* receivedMessage, int size); 

// for sending
void senderGetMessageToSend(List* plist, char* sendMessage);
void senderMessageToRemote(char* message);

#endif