emmalee : parser.o shell.o
	gcc -o emmalee parser.o shell.o
shell.o : shell.c parser.c
	gcc -c shell.c
parser.o : parser.c parser.h
	gcc -c parser.c
clean :
	rm *.o
.PHONY : clean