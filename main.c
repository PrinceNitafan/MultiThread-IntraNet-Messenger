#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "receiver_sender.h"
#include "threadSignal.h"


int main(int argc, char** args) {
    printf("Starting...\n");

    receive_send_init(args[1],args[2],args[3]);
    // sender_init(args[2],args[3]);

    threadInit();
    
    threadShutdown();
    printf("Shutdown done!\n");
    return 0;
}

