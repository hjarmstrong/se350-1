CC_FLAGS=-g -Wall -Wextra -std=c99

LIST=src/list/list.o
MEM=src/mem/mem.o
PROC=src/proc/process.o src/proc/sys_process.o src/proc/user_process.o
SVC=src/svc/hal.o
TEST=src/test/list_test.o src/test/mem_test.o

SRC=$(LIST) $(MEM) $(PROC) $(SVC) $(SVC) src/main.o src/printf.o src/uart_polling.o
SYS=sys/system_LPC17xx.o
OBJECTS=$(SRC) $(SYS)


.SUFFIXES:
.SUFFIXES: .o .c

.c.o:
	${CC} $(CC_FLAGS) -c $<


all: os


.PHONY: os
os: $(OBJECTS)
	${CC} $(CC_FLAGS) $^ $(OBJECTS) -o $@


clean:
	rm -rf *.dSYM
	rm -f src/list/*.o
	rm -f src/mem/*.o
	rm -f src/svc/*.o
	rm -f src/test/*.o
	rm -f src/*.o
	rm -f sys/*.o
