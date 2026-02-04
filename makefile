# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm

# Target executable
TARGET = canny

# Source files
SRC = canny.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	-cmd /c del /q $(TARGET).exe cannyout.pgm >nul 2>&1
	-rm -f $(TARGET) cannyout.pgm

run: $(TARGET)
	./$(TARGET) garb34.pgm cannyout.pgm 1.0
