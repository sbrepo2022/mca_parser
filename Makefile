INCPATH = C:/MinGW/msys/1.0/local/include
LIBPATH = C:/MinGW/msys/1.0/local/lib

all: uncomp.o
	gcc -o uncomp uncomp.o -L$(LIBPATH) -lz

uncomp.o: uncomp.cc
	gcc -c uncomp.cc -I$(INCPATH)