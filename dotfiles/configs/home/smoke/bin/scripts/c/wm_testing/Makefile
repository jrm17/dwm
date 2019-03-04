BIN = wm
SRC = wm.c
OBJ ?= ${SRC:.c=.o}

PREFIX = /usr

CC ?= cc
LD  = ${CC}
CFLAGS  += -Wall -I/opt/X11/include -I/usr/X11R6/include
LDFLAGS += -lxcb -L/opt/X11/lib -L/usr/X11R6/lib

.POSIX:
all: ${BIN}

.c.o: ${SRC}
	@echo "${CC} -c $< -o $@ ${CFLAGS}"
	@${CC} -c $< -o $@ ${CFLAGS}

${BIN}: ${OBJ}
	@echo "${LD} $^ -o $@ ${LDFLAGS}"
	@${LD} -o $@ ${OBJ} ${LDFLAGS}

install: ${BIN}
	mkdir -p ${DESTDIR}${PREFIX}/bin
	install ${BIN} ${DESTDIR}${PREFIX}/bin/${BIN}
	chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN}

uninstall: ${DESTDIR}${PREFIX}/bin/${BIN}
	rm -f ${DESTDIR}${PREFIX}/bin/${BIN}

clean:
	rm -f ${BIN} ${OBJ}
