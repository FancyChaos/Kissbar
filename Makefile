PREFIX = /usr/local
CC = cc

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

INCS = -I$(X11INC)
LIBS = -L$(X11LIB) -lX11

CFLAGS = -std=c99 -Wall -pedantic -Wconversion -O2

-include config.mk

all: kissbar

kissbar: kissbar.o
	$(CC) kissbar.o -o kissbar $(LIBS)

kissbar.o: kissbar.c kissbar.h
	$(CC) -c kissbar.c $(CFLAGS) $(INCS)

linux:
	cp config.mk.linux config.mk

install: kissbar
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp kissbar $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/kissbar

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/bin/kissbar

clean:
	rm -f *.o kissbar

.PHONY: all linux install uninstall clean
