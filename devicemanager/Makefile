CFLAGS	= -Wall -W -O2 -g
LIBS		= -pthread

all : dmtest

%.o: %.c	
	$(CC) $(CFLAGS) -c -o $*.o $<

dmtest : DeviceManager.o dmtest.o lmsens.o zyhd.o ../lib/sock.o
	cc $(CFLAGS) -o $@ $^ $(LIBS)