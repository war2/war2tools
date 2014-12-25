BIN = ppm_cmp
OBJ = ppm_cmp.o

CFLAGS := -O2 -Wall -W

.PHONY: clean

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(OBJ)
	$(RM) $(BIN)

