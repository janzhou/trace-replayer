CFLAGS	= -Wall -W -O2 -g
LIBS		= -pthread

all: server

server: sock_server.o sock.o sock_server_test.o
	cc $(CFLAGS) -o $@ $^ $(LIBS)
	
%.o: %.c	
	$(CC) $(CFLAGS) -c -o $*.o $<
clean : 
	rm -f *.o