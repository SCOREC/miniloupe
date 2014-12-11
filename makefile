CONFIG ?= default

include $(CONFIG).mk

BACK_LDLIBS = -lm

prefix ?= /you-didnt-set-prefix

all: test view

test: test.o libmilo.a
	$(BACK_CC) $(BACK_LDFLAGS) -o $@ $^ $(BACK_LDLIBS)

libmilo.a: milo.o proto__back.o proto_back.o from_mpi.o socks__back.o scene.o globe.o \
  render.o draw_text.o charbits.o image.o space.o base__back.o
	ar rcs $@ $^

view: view.o proto__front.o proto_front.o socks__front.o base__front.o
	$(FRONT_CC) -o $@ $^ $(GTK_LIBS)

install: milo.h libmilo.a test view
	mkdir -p $(prefix)/include
	mkdir -p $(prefix)/lib
	mkdir -p $(prefix)/bin
	cp milo.h $(prefix)/include
	cp libmilo.a $(prefix)/lib
	cp test view $(prefix)/bin

test.o: test.c milo.h from_mpi.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
base__back.o: base.c base.h
	$(BACK_CC) $(BACK_CFLAGS) -c $< -o $@
base__front.o: base.c base.h
	$(FRONT_CC) $(FRONT_CFLAGS) -c $< -o $@
charbits.o: charbits.c charbits.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
draw_text.o: draw_text.c image.h charbits.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
image.o: image.c image.h base.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
render.o: render.c image.h space.h render.h base.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
scene.o: scene.c image.h space.h render.h scene.h base.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
space.o: space.c space.h base.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
globe.o: globe.c space.h globe.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
view.o: view.c socks.h
	$(FRONT_CC) $(GTK_CFLAGS) $(FRONT_CFLAGS) -c $<
milo.o: milo.c milo.h base.h image.h space.h render.h scene.h globe.h \
 socks.h proto.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
socks__back.o: socks.c socks.h base.h
	$(BACK_CC) $(BACK_CFLAGS) -c $< -o $@
proto__back.o: proto.c proto.h socks.h from_mpi.h
	$(BACK_CC) $(BACK_CFLAGS) -c $< -o $@
socks__front.o: socks.c socks.h base.h
	$(FRONT_CC) $(FRONT_CFLAGS) -c $< -o $@
proto__front.o: proto.c proto.h socks.h from_mpi.h
	$(FRONT_CC) $(FRONT_CFLAGS) -c $< -o $@
proto_front.o: proto_front.c proto.h socks.h
	$(FRONT_CC) $(FRONT_CFLAGS) -c $<
proto_back.o: proto_back.c proto.h socks.h from_mpi.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<
from_mpi.o: from_mpi.c from_mpi.h image.h base.h
	$(BACK_CC) $(BACK_CFLAGS) -c $<

writeup.pdf: writeup.tex
	pdflatex writeup

clean:
	git clean -fdx

.PHONY: all clean install
