# Compiler and flags
CC = mpicc
CFLAGS = -Wall -Wextra -g -pedantic

# Libraries
LIBS = -llapacke -lblas -lm

COMMON_SRCS = linear_solver.c cyclic_serial.c tridiagonal_matrix.c aux.c 
# Source files
TEST_SRCS = test.c $(COMMON_SRCS) 
PARALLEL_SRCS = cyclic_parallel.c $(COMMON_SRCS) 

# Header files
HEADERS = linear_solver.h aux.h tridiagonal_matrix.h cyclic_serial.h

# Object files
TEST_OBJS = $(TEST_SRCS:.c=.o)
PARALLEL_OBJS = $(PARALLEL_SRCS:.c=.o)

# Targets
TEST_TARGET = test
PARALLEL_TARGET = cyclic_parallel

# Default target
all: $(TEST_TARGET) $(PARALLEL_TARGET)

# Build the test executable
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) $(LIBS)

$(PARALLEL_TARGET): $(PARALLEL_OBJS)
	$(CC) $(CFLAGS) -o $@ $(PARALLEL_OBJS) $(LIBS)

# Compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TEST_OBJS) $(TEST_TARGET) $(PARALLEL_OBJS) $(PARALLEL_TARGET)
run_test:
	./test

# PHONY targets
.PHONY: all clean

