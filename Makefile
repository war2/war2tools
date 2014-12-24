BIN = pudviewer
OBJ = file.o pud.o main.o
CFLAGS := -O2 -Wall -W

.PHONY: clean

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(OBJ)
	$(RM) $(BIN)

