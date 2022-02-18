CC=gcc
CCX=x86_64-w64-mingw32-gcc
CFLAGS=-Wall -Wextra -Wpedantic

pwdg: main.c
	$(CC) main.c -o pwdg $(CFLAGS)

pwdg.exe: main.c
	$(CCX) main.c -o pwdg.exe $(CFLAGS)
