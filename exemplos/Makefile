#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
BIN_DIR=../bin

all: exemplo mandel teste_vetor barbeiro filosofos prodcons series

exemplo: exemplo.c $(LIB_DIR)/libcthread.a
	$(CC) -o exemplo exemplo.c -L$(LIB_DIR) -lcthread -Wall

teste_vetor: teste_vetor.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_vetor teste_vetor.c -L$(LIB_DIR) -lcthread -Wall

mandel: mandel.c $(LIB_DIR)/libcthread.a
	$(CC) -o mandel mandel.c -L$(LIB_DIR) -lcthread -lm -Wall

barbeiro: barbeiro.c $(LIB_DIR)/libcthread.a
	$(CC) -o barbeiro barbeiro.c -L$(LIB_DIR) -lcthread -lm -Wall

filosofos: filosofos.c $(LIB_DIR)/libcthread.a
	$(CC) -o filosofos filosofos.c -L$(LIB_DIR) -lcthread -lm -Wall

prodcons: prodcons.c $(LIB_DIR)/libcthread.a
	$(CC) -o prodcons prodcons.c -L$(LIB_DIR) -lcthread -lm -Wall

series: series.c $(LIB_DIR)/libcthread.a
	$(CC) -o series series.c -L$(LIB_DIR) -lcthread -lm -Wall
	
library:
	$(CC) -o $(SRC_DIR)/lib.o -c $(SRC_DIR)/lib.c
	ar -rsc $(LIB_DIR)/libcthread.a $(SRC_DIR)/lib.o $(BIN_DIR)/support.o

clean:
	rm -rf exemplo mandel teste_vetor barbeiro filosofos prodcons series *.o *~
	rm $(SRC_DIR)/lib.o $(LIB_DIR)/libcthread.a


