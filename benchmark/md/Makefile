CC = gcc  #change compiler for mpi programs (mpicc)
CFLAGS = -c 
LDFLAGS = -lm
SOURCES = control.c MD.c util.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=benchmark
ARGS=

all: $(SOURCES) $(EXECUTABLE)
        
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm $(EXECUTABLE) $(OBJECTS)

run :
	@$(MAKE) && ./$(EXECUTABLE) $(ARGS)
