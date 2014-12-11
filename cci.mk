FRONT_CC = /usr/bin/gcc
BACK_CC = mpicc
FRONT_CFLAGS = -Wall -O3 -g -fPIC
BACK_CFLAGS = -g -O3 -qstrict
FRONT_LDFLAGS = -fPIC
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-2.0)
