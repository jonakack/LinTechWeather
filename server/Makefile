CC := gcc
CFLAGS = Wall -I../libs
LIBS_DIR = ../libs

LIB_SOURCES = $(wildcard $(LIBS_DIR)/*.c)
LIB_OBJS = $(LIB_SOURCES:.c=.o)

TARGET = weather_server
SRCS = main.c
OBJS = (SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS)	-c $< -o $@

$(LIBS_DIR)/%.o: $(LIBS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) $(LIB_OBJS)




