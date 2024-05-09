#ifndef _PRINTER_H_
#define _PRINTER_H_

#include "list.h"

void printerGetReceivedMessage(List* plist, char* receivedMessage);
void printerPrintReceivedMessage(char* receivedMessage);

#endif