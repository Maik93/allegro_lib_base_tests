#---------------------------------------------------
# Target file to be compiled by default
#---------------------------------------------------
MAIN = main
#---------------------------------------------------
# CC is the compiler to be used
#---------------------------------------------------
CC = gcc
#---------------------------------------------------
# CFLAGS are the options passed to the compiler
#---------------------------------------------------
CFLAGS = -Wall -lpthread -lrt -lm
#---------------------------------------------------
# OBJS are the object files to be linked
#---------------------------------------------------
#OBJ1 = 
OBJ2 = utils
OBJ3 = tasks
OBJS = $(MAIN).o $(OBJ2).o $(OBJ3).o
#---------------------------------------------------
# LIBS are the external libraries to be used
#---------------------------------------------------
LIBS = `allegro-config --libs`
#---------------------------------------------------
# Dependencies
#---------------------------------------------------
$(MAIN): $(OBJS)
	$(CC) -o $(MAIN) $(OBJS) $(LIBS) $(CFLAGS)
$(MAIN).o: $(MAIN).c
	$(CC) -c $(MAIN).c
#$(OBJ1).o: $(OBJ1).c
#	$(CC) -c $(OBJ1).c
$(OBJ2).o: $(OBJ2).c
	$(CC) -c $(OBJ2).c
$(OBJ3).o: $(OBJ3).c
	$(CC) -c $(OBJ3).c
#---------------------------------------------------
# Command that can be specified inline: make run
#---------------------------------------------------
run: $(MAIN)
	./$(MAIN)
#---------------------------------------------------
# Command that can be specified inline: make clean
#---------------------------------------------------
clean:
	rm -rf *.o $(MAIN)