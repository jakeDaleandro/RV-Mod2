# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm

# Target executable
TARGET = sobel

# Source files
SRC = Sobel.c

# Default target
all: $(TARGET)

# Build rule
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Remove generated files
clean:
	-cmd /c del /q $(TARGET).exe sobelout1.pgm sobelout2.pgm sobelmag.pgm >nul 2>&1
	# Keep this fallback for Unix-like shells
	-rm -f $(TARGET) sobelout1.pgm sobelout2.pgm sobelmag.pgm

# Convenience target: build + run (optional)
run: $(TARGET)
	./$(TARGET) garb34.pgm sobelmag.pgm 100
