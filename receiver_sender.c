#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "list.h"
#include "receiver_sender.h"

#define MSG_MAX_LEN 1024

static int socketReceiveSend;
static struct addrinfo* local_add;
static struct addrinfo* remote_add;

// creates the socket
void receive_send_init(char* recPort, char* remote_address, char* senPort) {
    struct addrinfo recHints, senHints,*record;
    int status;

    // This is for the receiving local addrinfo
    memset(&recHints, 0, sizeof(recHints));
    recHints.ai_family = AF_UNSPEC;  // Use either IPv4 or IPv6
    recHints.ai_socktype = SOCK_DGRAM;  // Use UDP
    recHints.ai_flags = AI_PASSIVE;  // Use local address

    status = getaddrinfo(NULL, recPort, &recHints, &local_add);
    if (status != 0) {
        perror("Receiving getaddrinfo error.");
        exit(1);
    }

    // This is for the sending remote addrinfo
    memset(&senHints, 0, sizeof(senHints));
    senHints.ai_family = AF_UNSPEC; // Use either IPv4 or IPv6
    senHints.ai_socktype = SOCK_DGRAM; // Use UDP

    status = getaddrinfo(remote_address, senPort, &senHints, &remote_add);
    if (status != 0) {
        perror("Sending getaddrinfo error.");
        exit(1);
    }
 
    // This creates the bind for local receiving
    record = local_add;
    while (record!= NULL) {
        // IPv4
        if (record->ai_family == AF_INET) {
            socketReceiveSend = socket(record->ai_family, SOCK_DGRAM, 0);
            if (socketReceiveSend != -1){
                bind(socketReceiveSend, record->ai_addr, record->ai_addrlen);
                break;
            }
        }

        // IPv6
        if (record->ai_family == AF_INET6) {
            socketReceiveSend = socket(record->ai_family, SOCK_DGRAM, 0);
            if (socketReceiveSend != -1){
                bind(socketReceiveSend, record->ai_addr, record->ai_addrlen);
                break;
            }
        }

        record = record->ai_next;
        if (record == NULL) {
            perror("Unable to create a socket and bind a connection for receiving.");
            exit(1);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// RECEIVING THREAD ////////////////////////////////////////////////////////

// This is for the receiving thread to get the new message
void receiverGetNewMessage(char* receivedMessage, int size) {
    struct sockaddr sinRemote;
    unsigned int sin_len = sizeof(sinRemote);
    
    char buffer[size];
    int bytes_received = recvfrom(socketReceiveSend, buffer, size, 0, (struct sockaddr *)&sinRemote, &sin_len);
    
    if (bytes_received == -1) {
        perror("recvfrom.");

    // If the received data is less than the max size (with the last character being terminating '\0')
    } else if (bytes_received <= 1023) {
        buffer[bytes_received] = '\0';
        strcpy(receivedMessage, buffer);
        // memcpy(receivedMessage, buffer, bytes_received);
        // receivedMessage[bytes_received] = '\0'; // Null-terminate the string
        // printf("hi there\n");

    // If the received data is too large, then it is an error
    } else {
        strcpy(receivedMessage, "Error: incoming message is too large.");
        // receivedMessage[1023] = '\0'; // Ensure null-termination
    }
}

// This is for the receving thread to add the new message to the receiveList
void receiverInputNewMessageToReceiveList(List* plist, void* receivedMessage) {

    char* newMessage = (char*)malloc((strlen(receivedMessage) + 1)*sizeof(char));
    strcpy(newMessage, receivedMessage);
    newMessage[strlen(receivedMessage)] = '\0';

    // prepends the new message to the receiveList
    unsigned int num = List_prepend(plist, newMessage);
    if (num == LIST_FAIL) {
        puts("Failed to add typed message to the send list.");
    }
}

//////////////////////////////////////////////////// RECEIVING THREAD ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// SENDING THREAD /////////////////////////////////////////////////////////

// This is for the sending thread to get the message from the sendList
void senderGetMessageToSend(List* plist, char* sendMessage) {

    // this is to get the oldest message from the sendList
    char* message = List_trim(plist);
    if (message == NULL) {
        strcpy(sendMessage, "Failed to get message from sendList.");
    } else{
        strcpy(sendMessage, message);
        free(message);
    }
}

// This is for the sending thread to send the message
void senderMessageToRemote(char* message) {

    int error = sendto(socketReceiveSend, message, strlen(message), 0, remote_add->ai_addr, remote_add->ai_addrlen);

    if (error == -1) perror("recvfrom.");
}

///////////////////////////////////////////////////// SENDING THREAD /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is to properly shutdown the sockets, to wake the sendto or recvfrom block, and to make sure memory leaks don't occur
void receive_sendShutdown() {
    shutdown(socketReceiveSend, SHUT_RDWR);
    close(socketReceiveSend);
    freeaddrinfo(local_add);
    freeaddrinfo(remote_add);
}


