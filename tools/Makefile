PPM_OBJ = ppm.o
PUD_OBJ = ../pud.o

CMP_BIN = ppm_cmp
CMP_OBJ = ppm_cmp.o

TILES_BIN = tiles
TILES_OBJ = tiles.o

CFLAGS := -O2 -Wall -W
LIBS := -ljpeg

.PHONY: clean

all: $(CMP_BIN) $(TILES_BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(CMP_BIN): $(PPM_OBJ) $(CMP_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(TILES_BIN): $(PPM_OBJ) $(PUD_OBJ) ../file.o ../jpeg.o $(TILES_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) $(CMP_OBJ)
	$(RM) $(CMP_BIN)
	$(RM) $(TILES_OBJ)
	$(RM) $(TILES_BIN)

