INSTALLDIR	:= /usr/local/bin
CC		:= cc


all: kissbar

kissbar: kissbar.o
	${CC} kissbar.o -o kissbar -L/usr/X11R6/lib -lX11

kissbar.o: kissbar.c kissbar.h
	cc -I/usr/X11R6/include -c kissbar.c

install: kissbar
	mkdir -p ${INSTALLDIR}
	cp -f kissbar ${INSTALLDIR}
	chmod 755 ${INSTALLDIR}/kissbar

uninstall:
	rm -f ${INSTALLDIR}/kissbar

clean:
	rm -rf *.o kissbar

.PHONY: all clean install uninstall
