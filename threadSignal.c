#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#include "threadSignal.h"
#include "receiver_sender.h"
#include "typeInputter.h"
#include "printer.h"

#define MAX_SIZE 1024

pthread_mutex_t accessToListNodes = PTHREAD_MUTEX_INITIALIZER; // provides the access to the nodes
pthread_mutex_t accessToSendList = PTHREAD_MUTEX_INITIALIZER; // provides the access to the sendList 
pthread_mutex_t accessToReceiveList = PTHREAD_MUTEX_INITIALIZER; // provides the access to the receiveList 

pthread_cond_t dataFull = PTHREAD_COND_INITIALIZER;          // variable for if there are no empty nodes left 
pthread_cond_t dataSendEmpty = PTHREAD_COND_INITIALIZER;     // variable for if there are no messages to send
pthread_cond_t dataReceiveEmpty = PTHREAD_COND_INITIALIZER;  // variable for if there are no received messages

pthread_t typeThread;
pthread_t sendThread;
pthread_t receiveThread;
pthread_t printThread;

static List* sendList;
static List* receiveList;

static char* typedMessage = NULL;
static char* sendMessageRemote = NULL;
static char* receivedMessage = NULL;
static char* printedMessage = NULL;
static char* forExitVar = "!";

static int exit_flag = 0;


// producer to the send list
void* typeMessage(void* arg) {
    while(exit_flag != 1) {

        // non-critical work
        // gets the input from the user using fgets()
        typing(typedMessage, MAX_SIZE);
        if (exit_flag == 1) break;

        pthread_mutex_lock(&accessToSendList);
        {
            // If there are no nodes available, continue to block
            while (List_count(sendList) + List_count(receiveList) == LIST_MAX_NUM_NODES && exit_flag != 1) {
                pthread_cond_wait(&dataFull, &accessToSendList);
            }
            // once the pthread_broadcast occurs, it will immediately unblock the mutex and break
            if (exit_flag == 1) {
                pthread_mutex_unlock(&accessToSendList);
                break;
            }

            // critical section
            pthread_mutex_lock(&accessToListNodes);
            {
                inputTypedMessageToList(sendList, typedMessage);
            }
            pthread_mutex_unlock(&accessToListNodes);     
            pthread_cond_signal(&dataSendEmpty); 
        }
        pthread_mutex_unlock(&accessToSendList); 

        // end condition for this thread if user types in !
        if (strcmp(typedMessage, forExitVar) == 0) break;
    }
    return NULL;
}


// consumer to the send list
void* sendMessage(void* arg) {
    while(exit_flag != 1) {

        pthread_mutex_lock(&accessToSendList);
        {
            // If there are no messages to send, continue to block
            while (List_count(sendList) == 0 && exit_flag != 1) {
                pthread_cond_wait(&dataSendEmpty, &accessToSendList);
            }
            // once the pthread_broadcast occurs, it will immediately unblock the mutex and break
            if (exit_flag == 1) {
                pthread_mutex_unlock(&accessToSendList);
                break;
            }
            
            // critical work
            pthread_mutex_lock(&accessToListNodes);
            {
                senderGetMessageToSend(sendList, sendMessageRemote);
            }
            pthread_mutex_unlock(&accessToListNodes);    
            pthread_cond_signal(&dataFull); 
        }
        pthread_mutex_unlock(&accessToSendList);

        // non critical work
        senderMessageToRemote(sendMessageRemote);

        // exit condition where once the user's message of "!" has been sent
        if (strcmp(sendMessageRemote, forExitVar) == 0){
            exit_flag = 1;
        }  
    }
    return NULL;
}


// producer to the receive list
void* receiveMessage(void* arg) {
    while(exit_flag != 1) {

        // non-critical work
        // gets the oldes received message
        receiverGetNewMessage(receivedMessage, MAX_SIZE);    
        if (exit_flag == 1) break;

        pthread_mutex_lock(&accessToReceiveList);
        {
            // If there are no empty nodes available, continue to block
            while (List_count(sendList) + List_count(receiveList) == LIST_MAX_NUM_NODES && exit_flag != 1) {
                pthread_cond_wait(&dataFull, &accessToReceiveList);
            }
            // once the pthread_broadcast occurs, it will immediately unblock the mutex and break
            if (exit_flag == 1) {
                pthread_mutex_unlock(&accessToReceiveList);
                break;
            }

            // critical work
            pthread_mutex_lock(&accessToListNodes);
            {
                receiverInputNewMessageToReceiveList(receiveList, receivedMessage);
            }
            pthread_mutex_unlock(&accessToListNodes); 
            pthread_cond_signal(&dataReceiveEmpty);
        }
        pthread_mutex_unlock(&accessToReceiveList);       
    }
    return NULL;
}


// consumer to the receive list
void* printMessage(void* arg) {
    while(exit_flag != 1) {
        pthread_mutex_lock(&accessToReceiveList);
        {
            // If there are no messages to print, continue to block
            while (List_count(receiveList) == 0 && exit_flag != 1) {
                pthread_cond_wait(&dataReceiveEmpty, &accessToReceiveList);
            }
            // once the pthread_broadcast occurs, it will immediately unblock the mutex and break
            if (exit_flag == 1) {
                pthread_mutex_unlock(&accessToReceiveList);
                break;
            }

            // critical work
            pthread_mutex_lock(&accessToListNodes);
            {
                printerGetReceivedMessage(receiveList, printedMessage);      
            }
            pthread_mutex_unlock(&accessToListNodes);
            pthread_cond_signal(&dataFull);
        }
        pthread_mutex_unlock(&accessToReceiveList);

        // non-critical work
        printerPrintReceivedMessage(printedMessage);

        // exit condition for if user2 has received a message of "!"
        if (strcmp(printedMessage, forExitVar) == 0){
            exit_flag = 1;
        }
    }
    return NULL;
}

// initializes the threads
void threadInit() {
    int result;
    sendList = List_create();
    receiveList = List_create();
    typedMessage = (char *)malloc(MAX_SIZE * sizeof(char));
    receivedMessage = (char *)malloc(MAX_SIZE * sizeof(char));
    sendMessageRemote = (char *)malloc(MAX_SIZE * sizeof(char));
    printedMessage = (char *)malloc(MAX_SIZE * sizeof(char));

    char var[] = "start";
    strcpy(typedMessage, var);
    strcpy(receivedMessage, var);
    strcpy(sendMessageRemote, var);
    strcpy(printedMessage, var);
    
    result = pthread_create(&typeThread, NULL, typeMessage, NULL);
    if (result != 0) {
        perror("Failed to create typeThread.");
        exit(EXIT_FAILURE);
    }

    result = pthread_create(&sendThread, NULL, sendMessage, NULL);
    if (result != 0) {
        perror("Failed to create sendThread.");
        exit(EXIT_FAILURE);
    }
    
    result = pthread_create(&receiveThread, NULL, receiveMessage, NULL);
    if (result != 0) {
        perror("Failed to create receiveThread.");
        exit(EXIT_FAILURE);
    }
    
    result = pthread_create(&printThread, NULL, printMessage, NULL);
    if (result != 0) {
        perror("Failed to create printThread.");
        exit(EXIT_FAILURE);
    }

    puts("System started...");

    // main threads continues indefinitely until the program is ended by the users
    while(exit_flag != 1) {
    }
}

//fake item deallocater
void item_deallocate(void* item) {
    free(item);
}

// properly shutdowns the threads
void threadShutdown() {
    // calls the shutdown for sockets
    receive_sendShutdown();

    // This will unblock all the threads waiting for the condition
    // Then, inside the thread, there will be an if statement to unlock the mutexes immediately
    pthread_cond_broadcast(&dataFull);
    pthread_cond_broadcast(&dataSendEmpty);
    pthread_cond_broadcast(&dataReceiveEmpty);

    // cancels the threads
    pthread_cancel(typeThread);
    pthread_cancel(sendThread);
    pthread_cancel(receiveThread);
    pthread_cancel(printThread);

    // the threads joins the main thread once the threads terminate
    pthread_join(typeThread, NULL);
    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);
    pthread_join(printThread, NULL);

    // clean up the allocated Lists and variables
    List_free(sendList, item_deallocate);
    List_free(receiveList, item_deallocate);
    free(typedMessage);
    free(receivedMessage);
    free(sendMessageRemote);
    free(printedMessage);

    // clean up the mutexes and the condition variables
    pthread_mutex_destroy(&accessToSendList);
    pthread_mutex_destroy(&accessToReceiveList);
    pthread_mutex_destroy(&accessToListNodes);
    pthread_cond_destroy(&dataFull);
    pthread_cond_destroy(&dataSendEmpty);
    pthread_cond_destroy(&dataReceiveEmpty);

    puts("Shutting down...");
}


