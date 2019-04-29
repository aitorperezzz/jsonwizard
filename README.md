# JSONwizard
Simple command line interface to manipulate JSON objects in C.

This is a command line program, written in C, which can help the programmer create, manipulate, load from file, write to file, interactively, objects in JSON notation.

The error checking is still not exhaustive, so discretion is advised.
The program uses an internal data structure to handle the data and can print it to the screen in a readable format.

Further work:
* Fix memory leaks that still happen.
* Ability to load from a .txt file.
* Ability to write to a .txt file.
* Develop the array type, which is supported in JSON but still not implemented.

Compile: 
`gcc -g JSONwizard.c -o JSONwizard`

Run: 
`./JSONwizard`
