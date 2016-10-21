server: server.o separator.o classifier.o handler.o client_data.o sender.o receiver.o connector.o binder.o
	gcc server.o separator.o classifier.o handler.o client_data.o sender.o receiver.o connector.o binder.o -o server -lpthread

server.o: server.c preclude/const.h preclude/separator.h preclude/classifier.h preclude/handler.h preclude/sender.h preclude/receiver.h  preclude/connector.h
	gcc -c server.c
	
separator.o: preclude/separator.c preclude/separator.h
	gcc -c preclude/separator.c

classifier.o: preclude/classifier.c preclude/classifier.h preclude/const.h
	gcc -c preclude/classifier.c
	
handler.o: preclude/handler.c preclude/handler.h preclude/const.h
	gcc -c preclude/handler.c

client_data.o: preclude/client_data.c preclude/client_data.h preclude/const.h
	gcc -c preclude/client_data.c
	
sender.o: preclude/sender.c preclude/sender.h
	gcc -c preclude/sender.c
	
receiver.o: preclude/receiver.c preclude/receiver.h
	gcc -c preclude/receiver.c
	
connector.o: preclude/connector.c preclude/connector.h
	gcc -c preclude/connector.c
	
binder.o: preclude/binder.c preclude/binder.h
	gcc -c preclude/binder.c

clean:
	@echo "cleanning project"
	-rm -f *.o
	@echo "clean completed"