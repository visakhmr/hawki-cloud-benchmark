CC = gcc
CFLAGS = -c -lm 
LDFLAGS =
SOURCES = benchmark.c
OBJECTS=$(SOURCES:.c=.o)
#DO not change the executable name
EXECUTABLE=benchmark
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
