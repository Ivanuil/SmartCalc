#include <stdbool.h>

#include "operand.h"

#ifndef STACK
#define STACK

#include "calc.h"

/// @brief Structure, representing a Stack's node (holds an Operand in it)
typedef struct StackNode {
  Operand operand;
  struct StackNode *next;
} StackNode;

/// @brief Structure, representing a stack data structure
///     (holds all the data, necessary to perform operations with stack)
typedef struct Stack {
  struct StackNode *top;
  int size;
} Stack;

/// @brief Initialize a Stack
/// @return Stack structure
Stack stackInit();

/// @brief Push Operand to Stack
/// @param stack Stack structure
/// @param operand Operand to push
void push(Stack *stack, Operand operand);

/// @brief Pop Operand from Stack
/// @param stack Stack struture
/// @return Popped Operand
Operand pop(Stack *stack);

#endif
