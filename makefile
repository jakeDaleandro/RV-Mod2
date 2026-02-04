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
	-cmd /c del /q $(TARGET).exe Final_Edges.pgm peaks.pgm magnitude.pgm >nul 2>&1
	-rm -f $(TARGET) Final_Edges.pgm peaks.pgm magnitude.pgm

run: $(TARGET)
	./$(TARGET) garb34.pgm Final_Edges.pgm peaks.pgm magnitude.pgm 1.0 30
