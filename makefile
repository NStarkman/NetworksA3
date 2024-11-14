CC = gcc
CFLAGS = -Wall -g

CLIENT_TARGET = sendFile
SERVER_TARGET = server

CLIENT_SRCS = TCPclient.c
SERVER_SRCS = TCPserver.c

CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJS)
    $(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS)

$(SERVER_TARGET): $(SERVER_OBJS)
    $(CC) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJS)

%.o: %.c
    $(CC) $(CFLAGS) -c $<

clean:
    rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET)

.PHONY: all clean