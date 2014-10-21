CFLAGS = -Wall -Werror -Wfatal-errors -Wpedantic -g -O3
PNG_CFLAGS = $(shell pkg-config --cflags libpng)
PNG_LIBS = $(shell pkg-config --libs libpng)
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-2.0)
LDLIBS += -lm

all: test view

test: test.o globe.o from_png.o render.o draw_text.o charbits.o image.o \
  space.o base.o
	$(CC) -O3 -o $@ $^ $(PNG_LIBS) $(LDLIBS)

view: view.o
	$(CC) -o $@ $^ $(GTK_LIBS) $(LDLIBS)

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
view.o: view.c
	$(CC) $(GTK_CFLAGS) $(CFLAGS) -c $<

clean:
	git clean -fdx
