CC = g++
CFLAGS = -Wall
PROG = sampleCollision

SRCS = Carom.cpp
LIBS = -lglut -lGL -lGLU

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LIBS)
	./a.out

clean:
	rm -f $(PROG)
