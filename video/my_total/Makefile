#
# http://www.gnu.org/software/make/manual/make.html
#
CC:=gcc
INCLUDES:=
CFLAGS:=-Wall -ggdb \
 -lm  -lavutil -lavformat -lavcodec -lz -lavutil -lm -lpthread -ldl  -lswresample -lx264 -lswscale -lmp3lame   -lrt   -g -O2 -D_REENTRANT -I/usr/local/include/SDL2   -DHAVE_OPENGL -g \
 -lSDL2_test -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -L/usr/local/lib -lSDL2
EXE:=tutorial01.out tutorial02.out  tutorial04.out\
	tutorial05.out tutorial06.out tutorial07.out

#
# This is here to prevent Make from deleting secondary files.
#
.SECONDARY:
	

#
# $< is the first dependency in the dependency list
# $@ is the target name
#
all: dirs $(addprefix bin/, $(EXE)) tags

dirs:
	mkdir -p obj
	mkdir -p bin

tags: *.c
	ctags *.c

bin/%.out: obj/%.o
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

obj/%.o : %.c
	$(CC) $(CFLAGS) $< $(INCLUDES) -c -o $@

clean:
	rm -f obj/*
	rm -f bin/*
	rm -f tags

