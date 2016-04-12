CC=g++
INCLUDE= ./include
CFLAGS=-I$(INCLUDE) -std=c++11
OBJDIR = ./obj
LDIR=./lib
LIBS=-lpthread
DEBUG=-g

_DEPS = aioheader.h
DEPS = $(patsubst %,$(INCLUDE)/%,$(_DEPS))

_OBJ = aiowrite2.o compute.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))


$(OBJDIR)/%.o: %.cpp $(DEPS)
	$(CC) -c $(DEBUG) -o $@ $< $(CFLAGS)

aiotest2: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean: 
	rm -f $(OBJDIR)/*.o core *~ $(INCLUDE)/*~
