#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "stack.h"

#ifndef CALC
#define CALC

#define BUF_SIZE 1024
#define OPERAND_MAX_LENGTH 16

#define BRACKET_OPEN 1
#define BRACKET_CLOSE 2
#define SUMMARY 3
#define DIFFERENCE 4
#define MULTIPLICATION 5
#define DIVISION 6
#define EXPONENTIATION 7
#define REMAINDER 8
#define PLUS 9
#define MINUS 10

#define COS 11
#define SIN 12
#define TAN 13
#define ACOS 14
#define ASIN 15
#define ATAN 16
#define SQRT 17
#define LN 18
#define LOG 19

#define VARX 20

/// @brief Converts mathematical expression to List of Operands
/// @param string Expression
/// @param error Pointer to boolean (true => error occurred)
/// @return List structure
List parseString(char *string, bool *error);

/// @brief Converts mathematical expression from infix notation to reverse
/// polish notation
/// @param infix List of infix notation Operands
/// @param error Pointer to boolean (true => error occurred)
/// @return List of polish notation Operands
struct List toPolish(struct List infix, bool *error);

/// @brief Converts a List of Operands to mathematical expression (string)
/// @param list List of Operands
/// @param delimiter Char to use as delimiter (use '\0' to write without
/// delimiter)
/// @return Pointer to a string (must be freed after use)
char *getAsString(struct List list, char delimiter);

/// @brief Calculates expression
/// @param polish List of Operands
/// @param error Pointer to boolean (true => error occurred)
/// @return Result
double calculate(struct List polish, bool *error);

/// @brief Calculates expression with x
/// @param polish List of Operands
/// @param x Value of X variable
/// @param error Pointer to boolean (true => error occurred)
/// @return Result
double calculateForX(struct List polish, double x, bool *error);

#endif