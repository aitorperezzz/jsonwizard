# JSONwizard
Simple command line interface to manipulate JSON objects in C.

This is a command line program, written in C, which can help the programmer create, manipulate, load from file, write to file, interactively, objects in JSON notation. The program starts a Read-Eval-Print-Loop with several commands supported.

The error checking is still not exhaustive, so discretion is advised.
The program uses an internal data structure to store the information about the JSON file. For example, if a JSON file is loaded, it is expected to conform to the standard, or else it will break.

## Utilities
### `append <child key> to <parent key>`
Add a child node to a parent node.

### `modify <key> set <field> <value>`
Modify a field in a node with the given key. Fields that can be modified in this way are: type, key and data.

### `delete [<key>]`
If no key is given, deletes the current root tree and starts a fresh one. If a key is given, deletes (recursively) the node from the tree.

### `write <filename>`
Write the current root tree to a file. Its name will be `filename.json`.

### `print [<key>]`
If a key is given, prints the selected node to stdin. If no key is given, prints the whole root tree to stdin.

### `load <filename>`
Receives a filename (with extension) and loads its JSON content to a root tree. It supposes that the JSON file has a perfectly correct format, else it will break.

### `help`
Prints the list of commands and how to use them.

### `quit`
Frees memory and quits the program.

## Compile
Move all these files to a single folder and use the Makefile to compile. In a Linux-based system just type:
`make`
To clean the `bin/` directory before compiling type
`make clean`

## Run
To run the program type
`./wizard`

## Further work:
* Some testing is being done because memory leaks still happen in error cases.
* Develop the array type, which is supported in JSON but still not implemented here.
* Prevent the user from adding a node with an already existing key.
* Give a default filename when the user does not give one.
