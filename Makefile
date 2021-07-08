pokerlab.so: utils.o pokerlab.o
	gcc -shared utils.o pokerlab.o -o pokerlab.so

utils.o: utils.h utils.c
	gcc -DNDEBUG -O3 -Wall -fPIC -I/usr/include/python3.8 -std=c18 -c utils.c -o utils.o

pokerlab.o: utils.h pokerlab.c
	gcc -DNDEBUG -O3 -Wall -fPIC -I/usr/include/python3.8 -std=c18 -c pokerlab.c -o pokerlab.o

clean:
	rm -f *.o

