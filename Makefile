CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

SRC = main.c biblioteca_grafos.c
OBJ = $(SRC:.c=.o)
TARGET = programa

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)