include config.mk

all: kissbar

kissbar: kissbar.o
	$(CC) kissbar.o -o kissbar $(LIBS)

kissbar.o: kissbar.c kissbar.h
	$(CC) -c kissbar.c $(CFLAGS)

install: kissbar
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp kissbar $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/kissbar

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/bin/kissbar

clean:
	rm -f *.o kissbar

.PHONY: all install uninstall clean
