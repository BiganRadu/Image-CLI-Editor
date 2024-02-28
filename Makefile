GCC=gcc
CFLAGS=-Wall -Wextra
TARGETS = utils.c options.c main.c 
OUTPUT = image_editor

build: 
	$(GCC) $(CFLAGS) $(TARGETS) -lm -o $(OUTPUT)
pack:
	zip -FSr 315CA_BiganRadu-Cristin_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(OUTPUT)

.PHONY: pack clean