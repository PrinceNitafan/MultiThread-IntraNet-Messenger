#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "printer.h"

void printerGetReceivedMessage(List* plist, char* receivedMessage) {

    char* newMessage = (char*)List_trim(plist);
    if (newMessage == NULL) {
        strcpy(receivedMessage, "Failed to get message from receiveList.");
    } else{
        strcpy(receivedMessage, newMessage);
        free(newMessage);
    }
}

void printerPrintReceivedMessage(char* receivedMessage) {

    char printStr[strlen("Remote client: ") + strlen(receivedMessage) + 1];
    strcpy(printStr, "Remote client: ");
    strcat(printStr, receivedMessage);

    puts(printStr);
}
