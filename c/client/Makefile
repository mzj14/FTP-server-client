client: client.o modify.o file_sender.o file_receiver.o connector.o binder.o reply_classifier.o
	gcc client.o modify.o file_sender.o file_receiver.o connector.o binder.o reply_classifier.o -o client
	
client.o: client.c preclude/modify.h preclude/file_sender.h preclude/file_receiver.h preclude/connector.h preclude/binder.h preclude/reply_classifier.h
	gcc -c client.c

modify.o: preclude/modify.c preclude/modify.h
	gcc -c preclude/modify.c

file_sender.o: preclude/file_sender.c preclude/file_sender.h
	gcc -c preclude/file_sender.c

file_receiver.o: preclude/file_receiver.c preclude/file_receiver.h
	gcc -c preclude/file_receiver.c

connector.o: preclude/connector.c preclude/connector.h
	gcc -c preclude/connector.c

binder.o:  preclude/binder.c preclude/binder.h
	gcc -c preclude/binder.c

reply_classifier.o: preclude/reply_classifier.c preclude/reply_classifier.h preclude/const.h
	gcc -c preclude/reply_classifier.c

clean:
	@echo "cleanning project"
	-rm -f *.o
	@echo "clean completed"