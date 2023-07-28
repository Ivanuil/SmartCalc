#include <stdbool.h>

#ifndef OPERAND
#define OPERAND

typedef union OpVal {
  double value;
  int operationCode;
} OpVal;

typedef struct Operand {
  bool isOperation;
  OpVal opVal;
} Operand;

#endif