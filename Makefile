CC_FLAGS=-g -Wall -Wextra -std=c90

MEM=src/mem/mem.o src/mem/test.o
SVC=src/svc/hal.o
SRC=$(MEM) $(SVC) src/main.o src/printf.o src/uart_polling.o
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


src/mem/mem.o: src/mem/mem.c src/mem/mem.h src/printf.h src/stdefs.h
src/mem/test.o: src/mem/test.c src/mem/test.h src/mem/mem.c src/mem/mem.h src/printf.h src/stdefs.h
src/main.o: src/main.c src/mem/mem.h src/printf.h src/stdefs.h src/uart_polling.h
src/printf.o: src/printf.c src/printf.h src/uart_polling.h
src/uart_polling.o: src/uart_polling.c src/uart_polling.h


clean:
	rm -rf *.dSYM
	rm -f src/mem/*.o
	rm -f src/svc/*.o
	rm -f src/*.o
	rm -f sys/*.o
