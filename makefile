CFLAGS = -Wall -Wshadow -Werror -Wfatal-errors -Wpedantic -g -O3
PNG_CFLAGS = $(shell pkg-config --cflags libpng)
PNG_LIBS = $(shell pkg-config --libs libpng)
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-2.0)
LDLIBS += -lm

all: test view

test: test.o libmilo.a
	$(CC) -O3 -o $@ $^ $(PNG_LIBS) $(LDLIBS)

libmilo.a: milo.o proto.o socks.o scene.o globe.o from_png.o render.o \
  draw_text.o charbits.o image.o space.o base.o
	ar rcs $@ $^

view: view.o proto.o socks.o base.o
	$(CC) -o $@ $^ $(GTK_LIBS) $(LDLIBS)

test.o: test.c milo.h
base.o: base.c base.h
charbits.o: charbits.c charbits.h
draw_text.o: draw_text.c image.h charbits.h
from_png.o: from_png.c from_png.h image.h base.h
	$(CC) $(PNG_CFLAGS) $(CFLAGS) -c $<
image.o: image.c image.h base.h
render.o: render.c render.h space.h image.h base.h
space.o: space.c space.h base.h
globe.o: globe.c space.h globe.h
view.o: view.c socks.h
	$(CC) $(GTK_CFLAGS) $(CFLAGS) -c $<
milo.o: milo.c milo.h base.h image.h space.h render.h scene.h globe.h \
 from_png.h
socks.o: socks.c socks.h base.h
proto.o: proto.c proto.h socks.h

clean:
	git clean -fdx
