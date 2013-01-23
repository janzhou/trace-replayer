CFLAGS	= -Wall -W -O2 -g 
LIBS		= -lrt -laio -lpthread -lm
ALL =  main

all: $(ALL)


clean : 
	rm -f *.o
	rm -f list/*.o
	rm -f main
	rm -f devicemanager/*.o
	rm -f lib/*.o
#	$(MAKE) -C workload/filebench clean

pclean : 
	rm -f *.o
#	rm -f list/*.o
	rm -f main
#	rm -f devicemanager/*.o
#	rm -f lib/*.o

%.o: %.c	
	$(CC) $(CFLAGS) -c -o $*.o $<

main :  trace.o time.o main.o replayer.o feedback.o model_predict.o list/list.o devicemanager/DeviceManager.o devicemanager/zyhd.o lib/sock.o
	cc $(CFLAGS) -o $@ $^ $(LIBS)

