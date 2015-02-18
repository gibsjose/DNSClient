IDIR =
CC = g++
CFLAGS = -I$(IDIR) -Wall -g -O0 -std=c++11

ODIR = obj
LDIR = ../lib
LIBS =

BIN = DNSClient

_DEPS =
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = DNSClient.o DNSPacket.o ConfigManager.o Record.o ExtendedRecord.o
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
