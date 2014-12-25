BIN = pudviewer
OBJ = file.o pud.o main.o jpeg.o
CFLAGS := -O2 -Wall -W
LIBS := -ljpeg

.PHONY: clean

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) $(OBJ)
	$(RM) $(BIN)

