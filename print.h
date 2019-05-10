#ifndef PRINT_H
#define PRINT_H

// Includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "JSONwizard.h"

// Prototypes of public functions.
int jsonPrintToFile(NODE *, char *);
int jsonPrintToStdin(NODE *, char *);

#endif
