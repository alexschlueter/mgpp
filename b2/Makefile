CFLAGS=-pthread -g
TARGETS=boundedBuffer parallel_reduce

all: $(TARGETS)

$(TARGETS):
		$(CC) -o $@ $@.c $(CFLAGS)

boundedBuffer: boundedBuffer.c
paralel_reduce: parallel_reduce.c
