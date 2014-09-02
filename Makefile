TARGET = bird
OBJS = Audio.o BirdGame.o BirdMain.o Sprite.o Video.o

${TARGET}: ${OBJS}
	g++ `sdl2-config --libs` -lm -o $@ $^

.cpp.o:
	g++ --std=c++11 `sdl2-config --cflags` -g -c -o $@ $<

all: ${TARGET}

clean:
	-rm -f ${OBJS}
	-rm -f ${TARGET}

distclean: clean

