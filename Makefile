BIN = pudviewer
OBJ = file.o pud.o main.o jpeg.o
CFLAGS := -O2 -Wall -W
LIBS := -ljpeg

.PHONY: clean tools

all: $(BIN) tools

tools:
	$(MAKE) -C tools

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	$(MAKE) -C tools $@
	$(RM) $(OBJ)
	$(RM) $(BIN)

