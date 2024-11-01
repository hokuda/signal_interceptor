all:	libsignal_interceptor.so persistent_kill segfault Sleep.class libmain.so

libsignal_interceptor.so:	signal_interceptor.o
	gcc -g -fPIC -shared -o libsignal_interceptor.so signal_interceptor.o

signal_interceptor.o:	signal_interceptor.c
	gcc -g -fPIC -c -O0 -fno-inline signal_interceptor.c

persistent_kill:	persistent_kill.c
	gcc -g -o persistent_kill persistent_kill.c

segfault:		segfault.c
	gcc -g -o segfault segfault.c

#LD_PRELOAD=./libdo_something_before_main.so cat

clean:;	rm -fr *.o *.so *class segfault persistent_kill

install:	libsignal_interceptor.so
	install -p -v libsignal_interceptor.so /usr/lib64/
