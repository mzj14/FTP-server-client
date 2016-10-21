client: client.o modify.o sender.o receiver.o connector.o binder.o separator.o classifier.o handler.o client_data.o
	gcc client.o modify.o sender.o receiver.o connector.o binder.o separator.o classifier.o handler.o client_data.o -o client
	
client.o: client.c preclude/modify.h preclude/sender.h preclude/receiver.h preclude/connector.h
	gcc -c client.c

modify.o: preclude/modify.c preclude/modify.h
	gcc -c preclude/modify.c

sender.o: preclude/sender.c preclude/sender.h
	gcc -c preclude/sender.c

receiver.o: preclude/receiver.c preclude/receiver.h
	gcc -c preclude/receiver.c

connector.o: preclude/connector.c preclude/connector.h
	gcc -c preclude/connector.c

binder.o:  preclude/binder.c preclude/binder.h
	gcc -c preclude/binder.c
	
separator.o: preclude/separator.c preclude/separator.h
	gcc -c preclude/separator.c

classifier.o: preclude/classifier.c preclude/classifier.h preclude/const.h
	gcc -c preclude/classifier.c

handler.o: preclude/handler.c preclude/handler.h preclude/const.h
	gcc -c preclude/handler.c

client_data.o: preclude/client_data.c preclude/client_data.h preclude/const.h
	gcc -c preclude/client_data.c

clean:
	@echo "cleanning project"
	-rm -f *.o
	@echo "clean completed"