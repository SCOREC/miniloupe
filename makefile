MPICC = mpicc
CFLAGS = -Wall -Wshadow -Werror -Wfatal-errors -g -O3 -fPIC
LDFLAGS = -O3 -fPIC
PNG_CFLAGS = $(shell pkg-config --cflags libpng)
PNG_LIBS = $(shell pkg-config --libs libpng)
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-2.0)
LDLIBS += -lm
prefix ?= /you-didnt-set-prefix

all: test view

test: test.o libmilo.a
	$(MPICC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

libmilo.a: milo.o proto.o proto_back.o from_mpi.o socks.o scene.o globe.o \
  render.o draw_text.o charbits.o image.o space.o base.o
	ar rcs $@ $^

view: view.o proto.o proto_front.o socks.o base.o
	$(CC) $(LDFLAGS) -o $@ $^ $(GTK_LIBS) $(LDLIBS)

install: milo.h libmilo.a test view
	mkdir -p $(prefix)/include
	mkdir -p $(prefix)/lib
	mkdir -p $(prefix)/bin
	cp milo.h $(prefix)/include
	cp libmilo.a $(prefix)/lib
	cp test view $(prefix)/bin

test.o: test.c milo.h from_mpi.h
base.o: base.c base.h
charbits.o: charbits.c charbits.h
draw_text.o: draw_text.c image.h charbits.h
from_png.o: from_png.c from_png.h image.h base.h
	$(CC) $(PNG_CFLAGS) $(CFLAGS) -c $<
image.o: image.c image.h base.h
render.o: render.c image.h space.h render.h base.h
scene.o: scene.c image.h space.h render.h scene.h base.h
space.o: space.c space.h base.h
globe.o: globe.c space.h globe.h
view.o: view.c socks.h
	$(CC) $(GTK_CFLAGS) $(CFLAGS) -c $<
milo.o: milo.c milo.h base.h image.h space.h render.h scene.h globe.h \
 socks.h proto.h
socks.o: socks.c socks.h base.h
proto.o: proto.c proto.h socks.h from_mpi.h
proto_front.o: proto_front.c proto.h socks.h
proto_back.o: proto_back.c proto.h socks.h from_mpi.h
from_mpi.o: from_mpi.c from_mpi.h image.h base.h
	$(MPICC) $(CFLAGS) -c $<

writeup.pdf: writeup.tex
	pdflatex writeup

clean:
	git clean -fdx

.PHONY: all clean install
