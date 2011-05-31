

#CFLAGS = -pg -pipe -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g
#LINKS = -pg -L/usr/local/lib -lpcre
CFLAGS = -pipe -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g

all : acsm

acsm : acsm.o
	gcc -o acsm acsm.o

acsm.o : acsm.h acsm.c 
	gcc -o acsm.o -c ${CFLAGS} acsm.c  

clean: 
	rm -f acsm
	rm -f *.o
