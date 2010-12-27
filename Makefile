all: zet.so

LUAINC=/usr/include/lua5.1
ZETDIR=/home/ayourtch/zf

CC=gcc
CFLAGS=-I$(ZETDIR)/include -I$(LUAINC) -L$(ZETDIR)/.libs -lzet

zet.so: zet.c
	$(CC) $(CFLAGS) -shared -o zet.so zet.c

clean:
	rm -f zet.so
