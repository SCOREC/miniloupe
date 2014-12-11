FRONT_CC = cc
BACK_CC = mpicc
FRONT_CFLAGS = -Wall -O3 -g -fPIC
BACK_CFLAGS = -Wall -O3 -g -fPIC
FRONT_LDFLAGS = -fPIC
BACK_LDFLAGS = -O3 -fPIC
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-2.0)
