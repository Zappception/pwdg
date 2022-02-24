CC=gcc
CCX=x86_64-w64-mingw32-gcc
CFLAGS=-Wall -Wextra -Wpedantic

all: pwdg pwdg.exe

clean: 
	rm pwdg pwdg.exe

pwdg:
	$(CC) main.c -o pwdg $(CFLAGS)
	
pwdg.exe:
	$(CCX) main.c -o pwdg.exe $(CFLAGS)
