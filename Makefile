INSTALLDIR	:= /usr/local/bin
CC		:= cc

all: kissbar

kissbar: kissbar.o
	${CC} kissbar.o -o kissbar

kissbar.o: kissbar.c kissbar.h
	cc -c kissbar.c

install: kissbar
	mkdir -p ${INSTALLDIR}
	cp -f kissbar ${INSTALLDIR}
	chmod 755 ${INSTALLDIR}/kissbar

uninstall:
	rm -f ${INSTALLDIR}/kissbar

clean:
	rm -rf *.o kissbar

.PHONY: all clean install uninstall
