TARGET = bird
OBJS = BirdMain.o Sprite.o

${TARGET}: ${OBJS}
	g++ `sdl2-config --libs` -lSDL2_image -o $@ $<

.cpp.o:
	g++ `sdl2-config --cflags` -c -o $@ $<

all: ${TARGET}

clean:
	-rm -f ${OBJS}
	-rm -f ${TARGET}

distclean: clean

