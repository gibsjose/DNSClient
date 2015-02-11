IDIR =
CC = g++
CFLAGS = -I$(IDIR) -Wall -g -std=c++0x

ODIR = obj
LDIR = ../lib
LIBS = -lpthread

BIN = DNSClient

_DEPS =
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = DNSClient.o DNSPacket.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: setup $(BIN)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean setup

setup:
	@mkdir -p $(ODIR)

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN)
