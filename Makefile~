tail: tail.o 
	gcc -g tail.o -o tail

tail.o: tail.c 
	gcc -g -c -Wall tail.c -o tail.o

clean:
	rm -f *.o *.gch tail
