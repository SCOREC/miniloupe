CFLAGS = -Wall -Werror -Wfatal-errors -Wpedantic -g -O3
PNG_CFLAGS = $(shell pkg-config --cflags libpng)
PNG_LDLIBS = $(shell pkg-config --libs libpng)
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)
GTK_LDLIBS = $(shell pkg-config --libs gtk+-2.0)
LDLIBS += -lm

all: test base.o charbits.o draw_text.o from_png.o image.o render.o space.o \
  globe.o

test: test.o globe.o from_png.o render.o draw_text.o charbits.o image.o \
  space.o base.o
	$(CC) -o $@ $^ $(PNG_LDLIBS) $(LDLIBS)

test.o: test.c image.h space.h render.h from_png.h
base.o: base.c base.h
charbits.o: charbits.c charbits.h
draw_text.o: draw_text.c image.h charbits.h
from_png.o: from_png.c from_png.h image.h base.h
	$(CC) $(PNG_CFLAGS) $(CFLAGS) -c $<
image.o: image.c image.h base.h
render.o: render.c render.h space.h image.h base.h
space.o: space.c space.h base.h
globe.o: globe.c space.h globe.h

clean:
	git clean -fdx
