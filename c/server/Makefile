server: server.o separator.o classifier.o handler.o client_data.o file_sender.o file_receiver.o connector.o binder.o debug.o command_processor.o file_accessor.o
	gcc server.o separator.o classifier.o handler.o client_data.o file_sender.o file_receiver.o connector.o binder.o debug.o command_processor.o file_accessor.o -o server -lpthread

server.o: server.c preclude/const.h preclude/separator.h preclude/classifier.h preclude/handler.h preclude/file_sender.h preclude/file_receiver.h  preclude/connector.h preclude/debug.h preclude/command_processor.h
	gcc -c server.c
	
separator.o: preclude/separator.c preclude/separator.h preclude/debug.h
	gcc -c preclude/separator.c

classifier.o: preclude/classifier.c preclude/const.h preclude/classifier.h preclude/debug.h
	gcc -c preclude/classifier.c

client_data.o: preclude/client_data.c preclude/client_data.h preclude/const.h preclude/debug.h
	gcc -c preclude/client_data.c
	
file_sender.o: preclude/file_sender.c preclude/file_sender.h preclude/debug.h
	gcc -c preclude/file_sender.c
	
file_receiver.o: preclude/file_receiver.c preclude/file_receiver.h preclude/debug.h
	gcc -c preclude/file_receiver.c
	
connector.o: preclude/connector.c preclude/connector.h preclude/debug.h
	gcc -c preclude/connector.c
	
binder.o: preclude/binder.c preclude/debug.h preclude/const.h
	gcc -c preclude/binder.c

command_processor.o: preclude/command_processor.c preclude/command_processor.h preclude/const.h
	gcc -c preclude/command_processor.c preclude/const.h

debug.o: preclude/debug.c preclude/debug.h
	gcc -c preclude/debug.c

handler.o: preclude/handler.c preclude/handler.h preclude/const.h preclude/file_accessor.h preclude/debug.h
	gcc -c preclude/handler.c

file_accessor.o: preclude/file_accessor.c preclude/file_accessor.h
	gcc -c preclude/file_accessor.c