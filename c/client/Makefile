client: client.o modify.o file_sender.o file_receiver.o connector.o binder.o reply_classifier.o command_processor.o param_getter.o request_examiner.o file_accessor.o
	gcc client.o modify.o file_sender.o file_receiver.o connector.o binder.o reply_classifier.o command_processor.o param_getter.o request_examiner.o file_accessor.o -o client
	
client.o: client.c preclude/modify.h preclude/file_sender.h preclude/file_receiver.h preclude/connector.h preclude/binder.h preclude/reply_classifier.h preclude/param_getter.h preclude/request_examiner.h
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

param_getter.o: preclude/param_getter.c preclude/param_getter.h
	gcc -c preclude/param_getter.c

reply_classifier.o: preclude/reply_classifier.c preclude/reply_classifier.h preclude/const.h
	gcc -c preclude/reply_classifier.c
	
request_examiner.o: preclude/request_examiner.c preclude/request_examiner.h preclude/file_accessor.h preclude/const.h
	gcc -c preclude/request_examiner.c

command_processor.o: preclude/command_processor.c preclude/command_processor.h preclude/const.h
	gcc -c preclude/command_processor.c

file_accessor.o: preclude/file_accessor.c preclude/file_accessor.h
	gcc -c preclude/file_accessor.c
	
clean:
	@echo "cleanning project"
	-rm -f *.o
	@echo "clean completed"