EXECBIN  = memory
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:%.c=%.o)
FORMATS  = $(SOURCES:%.c=%.fmt)

CC       = clang
FORMAT   = clang-format
CFLAGS   = -Wall -Werror -Wextra -Wpedantic -Wstrict-prototypes

.PHONY: all clean format

all: $(EXECBIN)

$(EXECBIN): $(OBJECTS)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXECBIN) $(OBJECTS)

format: $(FORMATS)

%.fmt: %.c
	$(FORMAT) -i $<
	touch $@

checkMemoryget: 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s printf "get\nexample.txt\n" |./memory > result6.txt

checkMemoryset: 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./memory

