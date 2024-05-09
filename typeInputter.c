#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "typeInputter.h"

void typing(char* retStr, int size) {
    char str[size];
    fgets(str, size, stdin);
    
    // add the null terminator
    if (str[strlen(str)-1] != '\0') {
        str[strlen(str)-1] = '\0';
        strcpy(retStr, str);
    } else {
        strcpy(retStr, str);
    }
}

void inputTypedMessageToList(List* pList, char* typedMessage) {

    // mallocs a new variable to store inside the node
    char* newMessage = (char*)malloc((strlen(typedMessage) + 1)*sizeof(char));
    strcpy(newMessage, typedMessage);

    // prepends the new message
    unsigned int num = List_prepend(pList, newMessage);
    if (num == LIST_FAIL) {
        puts("Failed to add typed message to the send list.");
    }
}