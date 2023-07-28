#include "stack.h"

Stack stackInit() {
  Stack stack = {0};
  stack.size = 0;
  stack.top = NULL;

  return stack;
}

void push(Stack *stack, Operand operand) {
  StackNode *node = malloc(sizeof(StackNode));
  node->next = stack->top;
  node->operand = operand;
  stack->top = node;
  stack->size++;
}

Operand pop(Stack *stack) {
  Operand result = stack->top->operand;

  StackNode *node = stack->top;
  stack->top = node->next;
  stack->size--;
  free(node);

  return result;
}
