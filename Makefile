CFLAGS=-static
#CFLAGS=-g -fsanitize=address -fsanitize=undefined -fsanitize=unreachable -fsanitize=null -fsanitize=return -fno-omit-frame-pointer
DESTDIR=/

build: myip

install:
	mkdir -p ${DESTDIR}/usr/bin ${DESTDIR}/usr/lib/systemd/system
	install -m 755 myip ${DESTDIR}/usr/bin/
	install -m 644 myip.service ${DESTDIR}/usr/lib/systemd/system/

.PHONY: install
