#!/bin/bash

gcc -DNDEBUG -O3 -Wall -fPIC -I/usr/include/python3.8 -std=c18 -c pokerlab.c -o pokerlab.o
gcc -shared pokerlab.o -o pokerlab.so

