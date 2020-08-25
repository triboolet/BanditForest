#--------------------------------------------------------------
# Makefile generated with cmak version.
# Date: 25/7/2020 16:34:36
#--------------------------------------------------------------

PREFIX  = /usr/local
CFLAGS  = -Wall -Wextra -O3 -std=c++17
LDFLAGS = 

CC = g++
RM = rm -f
INSTALL_PROG = install -m 755 -s

EXE = main

OBJS = objs/BanditForest.o objs/consts.o objs/Dataset.o objs/main.o objs/OneVariableStump.o objs/RBOCPD.o objs/Tree.o objs/TwoVariablesStump.o objs/utils.o

ALL : $(EXE)

objs/BanditForest.o : BanditForest.cpp
	$(CC) -c BanditForest.cpp $(CFLAGS) -o objs/BanditForest.o

objs/consts.o : consts.cpp
	$(CC) -c consts.cpp $(CFLAGS) -o objs/consts.o

objs/Dataset.o : Dataset.cpp
	$(CC) -c Dataset.cpp $(CFLAGS) -o objs/Dataset.o

objs/main.o : main.cpp
	$(CC) -c main.cpp $(CFLAGS) -o objs/main.o

objs/OneVariableStump.o : OneVariableStump.cpp
	$(CC) -c OneVariableStump.cpp $(CFLAGS) -o objs/OneVariableStump.o

objs/RBOCPD.o : RBOCPD.cpp
	$(CC) -c RBOCPD.cpp $(CFLAGS) -o objs/RBOCPD.o

objs/Tree.o : Tree.cpp
	$(CC) -c Tree.cpp $(CFLAGS) -o objs/Tree.o

objs/TwoVariablesStump.o : TwoVariablesStump.cpp
	$(CC) -c TwoVariablesStump.cpp $(CFLAGS) -o objs/TwoVariablesStump.o

objs/utils.o : utils.cpp
	$(CC) -c utils.cpp $(CFLAGS) -o objs/utils.o

$(EXE) : $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(LDFLAGS)

install : $(EXE)
	$(INSTALL_PROG) $(EXE) $(PREFIX)/bin

uninstall :
	$(RM) $(PREFIX)/bin/$(EXE)

clean :
	$(RM) $(OBJS) $(EXE)
