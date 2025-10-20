//Flaggor och kompliator
CC := gcc
INCLUDE := -I../libs -I/mingw64/include 
LIB := -L/mingw64/lib -Linclude/libs
CFLAGS := -Wall -Wextra -MMD -MP -std=c11 $(INCLUDE)

SRCS := main.c
OBJS := $(SRCS:.c=.o)

TARGET := weather_client

//Standardmål
all: $(TARGET)

//Målfil
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)

run: $(TARGET)
	./$(TARGET)


.PHONY: all run clean