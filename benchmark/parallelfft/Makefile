CC = gcc
CFLAGS = -c -lm 
LDFLAGS =
SOURCES = fft_benchmark.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=fft_benchmark
ARGS=data.txt

all: $(SOURCES) $(EXECUTABLE)
        
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -lm -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm $(EXECUTABLE) $(OBJECTS)

run :
	@$(MAKE) && ./$(EXECUTABLE) $(ARGS)
