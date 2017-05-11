# Makefile to build the SDL tests

srcdir  = .

CC      = gcc
EXE	= 
CFLAGS  = -g -O2 -D_REENTRANT -I/usr/local/include/SDL2   -DHAVE_OPENGL -g
LIBS	=  -lSDL2_test -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -L/usr/local/lib -lSDL2  

sdl2: $(srcdir)/sdl2.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

