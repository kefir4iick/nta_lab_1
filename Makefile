CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -D_POSIX_C_SOURCE=199309L

COMMON_OBJS = \
    helper.o \
    prime.o \
    pollard.o \
    bm.o 

MAIN_OBJS = \
    main.o \
    canonical.o \
    $(COMMON_OBJS)

BENCH_OBJS = \
    bench.o \
    $(COMMON_OBJS)

TARGET_MAIN  = nta
TARGET_BENCH = bench

all: $(TARGET_MAIN) $(TARGET_BENCH)

$(TARGET_MAIN): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJS) -lm

$(TARGET_BENCH): $(BENCH_OBJS)
	$(CC) $(CFLAGS) -o $@ $(BENCH_OBJS) -lm

%.o: %.c nta.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET_MAIN) $(TARGET_BENCH)
