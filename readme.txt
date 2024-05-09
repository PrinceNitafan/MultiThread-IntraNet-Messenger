Author: Prince Emmanuel Nitafan
Email: pen@sfu.ca

Summary: This program is a Intra Network UDP Messenger with multiple threads to handle typing, sending, receiving, printing. Additionally, it handles a data critical section appropriately for all threads.
   

The max string length is 1023 (+1 for the null terminator).

Note: this code must run in an intra network system.

How to run with 1 devce: Compile the code through "make", and then run "./s-talk [receivingPort server] [remote_address] [sendingPort server]" 
		         on a terminal. On a second terminal, run "./s-talk [receivingPort server] [remote_address] [sendingPort server]". The first
			 s-talk's receivingPort value should be the same as the second s-talk's sendingPort value. Similarily, the first s-talk's
                         sendingPort value should be the same as the second s-talk's receivingPort value. The remote address is the ip address/

How to run with 2 devices: On the first device, compile the code through "make", and then run "./s-talk [receivingPort server] [remote_address] [sendingPort server]" 
			   on a terminal. On the second device, open a terminal and run "./s-talk [receivingPort server] [remote_address] [sendingPort server]". 
                           The first s-talk's receivingPort value should be the same as the second s-talk's sendingPort value. Similarily, the first s-talk's
                           sendingPort value should be the same as the second s-talk's receivingPort value. The first device's remote_address value should be the ip
                           address of the second device, and vice versa.

While running, typing and entering a message on one terminal will show up the second terminal. To end the connection and program, type in "!": it will end each side's
program.
