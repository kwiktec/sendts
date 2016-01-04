C      = gcc
LDFLAGS += -pthread -lrt

OBJS = kwik_udp_send.o
TARGET = kwik_udp_send
DESTDIR ?= /usr/local/bin/

all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
