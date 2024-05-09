
CFLAGS = -Wall -g -D _POSIX_C_SOURCE=200809L -Werror

all: build

build:
	gcc $(CFLAGS) main.c list.c threadSignal.c receiver_sender.c printer.c typeInputter.c -pthread -o s-talk

# run: build	
# 	./s-talk

# valgrind: build
# 	valgrind --leak-check=full ./s-talk

clean:
	rm s-talk