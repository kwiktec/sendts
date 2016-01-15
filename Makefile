C = gcc
LDFLAGS += -pthread -lrt

OBJS = sendts.o
TARGET = sendts
DESTDIR ?= /usr/local/bin

all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
