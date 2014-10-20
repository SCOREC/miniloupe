all: base.o charbits.o draw_text.o from_png.o image.o render.o space.o
base.o: base.c base.h
charbits.o: charbits.c charbits.h
draw_text.o: draw_text.c image.h charbits.h
from_png.o: from_png.c from_png.h image.h base.h
image.o: image.c image.h base.h
render.o: render.c render.h space.h image.h base.h
space.o: space.c space.h base.h
clean:
	rm -f *.o
