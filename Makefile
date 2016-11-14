CC = gcc
CFLAGS = -std=c99 -Wall -g -Wextra -Werror -pedantic

OBJS = main.o 
OBJS += fct.o 
OBJS += thresh1.o 
OBJS += cat.o 
OBJS += tree.o

MAIN = my_read_iso
VPATH = src tests
ISO = tests/myiso.iso
.PHONY : library clean all check debug

all : $(MAIN)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)
library : $(LIBR)

$(ISO) :
	mkisofs -o $@ .

check: $(ISO) all
	./tests/tests.sh ${ARG1} $(ISO) 
debug: CFLAGS += -g 
debug: check

$(MAIN) : $(OBJS)

$(LIBR) : $(LIB)($(OBJS))

clean :
	${RM} ${OBJS} ${LIB} $(MAIN) tests/myiso.iso
