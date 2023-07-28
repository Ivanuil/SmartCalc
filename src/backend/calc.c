#include "calc.h"

bool isNumber(char c);
Operand readOperation(char **string, char *stringStart, Operand lastRead,
                      bool *error);
void writeOperation(char **string, Operand operand);
bool isFunction(Operand operand);
int comparePriority(Operand o1, Operand o2);
int getPriority(Operand operand);
Operand perform(Operand operand, Stack *stack, bool *error);
bool isX(Operand operand);

List parseString(char *string, bool *error) {
  List result = listInit();
  *error = false;
  char *stringStart = string;
  while (string[0] != '\0' && !(*error)) {
    while (string[0] == ' ') string++;
    if (string[0] == '\0') break;
    if (isNumber(string[0])) {
      int i = 0;
      char temp[BUF_SIZE] = "";
      while (isNumber(string[0])) {
        temp[i] = string[0];
        temp[i + 1] = '\0';
        string++;
        i++;
      }
      Operand operand = {0};
      operand.isOperation = false;
      double value = 0;
      if (sscanf(temp, "%lf", &value) != 1) *error = true;
      operand.opVal.value = value;
      addToEnd(&result, operand);
    } else {
      if (result.size != 0)
        addToEnd(&result, readOperation(&string, stringStart,
                                        result.last->operand, error));
      else {
        Operand operand = {0};
        addToEnd(&result, readOperation(&string, stringStart, operand, error));
      }
    }
  }

  return result;
}

struct List toPolish(struct List infix, bool *error) {
  *error = false;
  List result = listInit();
  Stack stack = stackInit();
  ListNode *infixNode = infix.first;

  while (infixNode != NULL && !(*error)) {
    Operand operand = infixNode->operand;
    if (!operand.isOperation || isX(operand)) {
      addToEnd(&result, operand);
    } else if (isFunction(operand) ||
               operand.opVal.operationCode == BRACKET_OPEN) {
      push(&stack, operand);
    } else if (operand.opVal.operationCode == BRACKET_CLOSE) {
      while (stack.size != 0 &&
             stack.top->operand.opVal.operationCode != BRACKET_OPEN)
        addToEnd(&result, pop(&stack));
      if (stack.size == 0) {
        *error = true;
        break;
      }
      pop(&stack);
    } else {
      while (stack.size != 0 &&
             stack.top->operand.opVal.operationCode != BRACKET_OPEN &&
             comparePriority(stack.top->operand, operand) != -1)
        addToEnd(&result, pop(&stack));
      push(&stack, operand);
    }

    infixNode = infixNode->next;
  }
  while (stack.size != 0) {
    if (stack.top->operand.opVal.operationCode == BRACKET_OPEN ||
        stack.top->operand.opVal.operationCode == BRACKET_CLOSE)
      *error = true;
    addToEnd(&result, pop(&stack));
  }

  return result;
}

char *getAsString(struct List list, char delimiter) {
  int stringSize = (list.size + 1) * OPERAND_MAX_LENGTH;
  char *string = malloc(sizeof(char) * stringSize);
  char *ptr = string;
  ptr[0] = '\0';
  for (ListNode *node = list.first;
       node != NULL && ptr != string + stringSize - OPERAND_MAX_LENGTH;
       node = node->next) {
    Operand operand = node->operand;
    if (operand.isOperation && !isX(operand)) {
      writeOperation(&ptr, operand);
    } else {
      if (!isX(operand)) {
        char temp[BUF_SIZE] = "";
        sprintf(temp, "%lf", operand.opVal.value);
        strcpy(ptr, temp);
        ptr += strlen(temp);
      } else {
        char temp[BUF_SIZE] = "X";
        strcpy(ptr, temp);
        ptr += strlen(temp);
      }
    }
    if (delimiter != '\0') *(ptr++) = delimiter;
    ptr[0] = '\0';
  }

  if (delimiter != '\0' && ptr > string) ptr[-1] = '\0';
  return string;
}

double calculate(struct List polish, bool *error) {
  return calculateForX(polish, 0.0, error);
}

double calculateForX(struct List polish, double x, bool *error) {
  *error = false;
  Stack stack = stackInit();
  ListNode *node = polish.first;
  while (node != NULL && *error != true) {
    Operand operand = node->operand;
    if (operand.isOperation && operand.opVal.operationCode == VARX) {
      operand.isOperation = false;
      operand.opVal.value = x;
      push(&stack, operand);
    } else if (!operand.isOperation)
      push(&stack, operand);
    else
      push(&stack, perform(operand, &stack, error));

    node = node->next;
  }
  if (stack.size != 1) {
    *error = true;
    while (stack.size != 0) pop(&stack);
    return 0;
  }
  return pop(&stack).opVal.value;
}

// UTILITARY FUNCTIONS

bool isNumber(char c) {
  if ((c >= '0' && c <= '9') || c == '.') return true;
  return false;
}

Operand readOperation(char **string, char *stringStart, Operand lastRead,
                      bool *error) {
  Operand result = {0};
  result.isOperation = true;

  if ((*string)[0] == 'x' || (*string)[0] == 'X') {
    result.opVal.operationCode = VARX;
    (*string)++;
  } else if ((*string)[0] == '+') {
    if (*string == stringStart ||
        (lastRead.isOperation && !isX(lastRead) &&
         lastRead.opVal.operationCode != BRACKET_CLOSE))
      result.opVal.operationCode = PLUS;
    else
      result.opVal.operationCode = SUMMARY;
    (*string)++;
  } else if ((*string)[0] == '-') {
    if (*string == stringStart ||
        (lastRead.isOperation && !isX(lastRead) &&
         lastRead.opVal.operationCode != BRACKET_CLOSE))
      result.opVal.operationCode = MINUS;
    else
      result.opVal.operationCode = DIFFERENCE;
    (*string)++;
  } else if ((*string)[0] == '*') {
    result.opVal.operationCode = MULTIPLICATION;
    (*string)++;
  } else if ((*string)[0] == '/') {
    result.opVal.operationCode = DIVISION;
    (*string)++;
  } else if ((*string)[0] == '^') {
    result.opVal.operationCode = EXPONENTIATION;
    (*string)++;
  } else if ((*string)[0] == '(') {
    result.opVal.operationCode = BRACKET_OPEN;
    (*string)++;
  } else if ((*string)[0] == ')') {
    result.opVal.operationCode = BRACKET_CLOSE;
    (*string)++;
  } else if (strncmp(*string, "cos", 3) == 0) {
    result.opVal.operationCode = COS;
    *string += 3;
  } else if (strncmp(*string, "sin", 3) == 0) {
    result.opVal.operationCode = SIN;
    *string += 3;
  } else if (strncmp(*string, "tan", 3) == 0) {
    result.opVal.operationCode = TAN;
    *string += 3;
  } else if (strncmp(*string, "acos", 4) == 0) {
    result.opVal.operationCode = ACOS;
    *string += 4;
  } else if (strncmp(*string, "asin", 4) == 0) {
    result.opVal.operationCode = ASIN;
    *string += 4;
  } else if (strncmp(*string, "atan", 4) == 0) {
    result.opVal.operationCode = ATAN;
    *string += 4;
  } else if (strncmp(*string, "sqrt", 4) == 0) {
    result.opVal.operationCode = SQRT;
    *string += 4;
  } else if (strncmp(*string, "ln", 2) == 0) {
    result.opVal.operationCode = LN;
    *string += 2;
  } else if (strncmp(*string, "log", 3) == 0) {
    result.opVal.operationCode = LOG;
    *string += 3;
  } else if (strncmp(*string, "mod", 3) == 0) {
    result.opVal.operationCode = REMAINDER;
    *string += 3;
  } else {
    *error = true;
  }

  return result;
}

void writeOperation(char **string, Operand operand) {
  if (operand.opVal.operationCode == SUMMARY) {
    (*string)[0] = '+';
    (*string)++;
  } else if (operand.opVal.operationCode == DIFFERENCE) {
    (*string)[0] = '-';
    (*string)++;
  } else if (operand.opVal.operationCode == MULTIPLICATION) {
    (*string)[0] = '*';
    (*string)++;
  } else if (operand.opVal.operationCode == DIVISION) {
    (*string)[0] = '/';
    (*string)++;
  } else if (operand.opVal.operationCode == EXPONENTIATION) {
    (*string)[0] = '^';
    (*string)++;
  } else if (operand.opVal.operationCode == BRACKET_OPEN) {
    (*string)[0] = '(';
    (*string)++;
  } else if (operand.opVal.operationCode == BRACKET_CLOSE) {
    (*string)[0] = ')';
    (*string)++;
  } else if (operand.opVal.operationCode == COS) {
    strcpy(*string, "cos");
    (*string) += 3;
  } else if (operand.opVal.operationCode == SIN) {
    strcpy(*string, "sin");
    (*string) += 3;
  } else if (operand.opVal.operationCode == TAN) {
    strcpy(*string, "tan");
    (*string) += 3;
  } else if (operand.opVal.operationCode == ACOS) {
    strcpy(*string, "acos");
    (*string) += 4;
  } else if (operand.opVal.operationCode == ASIN) {
    strcpy(*string, "asin");
    (*string) += 4;
  } else if (operand.opVal.operationCode == ATAN) {
    strcpy(*string, "atan");
    (*string) += 4;
  } else if (operand.opVal.operationCode == SQRT) {
    strcpy(*string, "sqrt");
    (*string) += 4;
  } else if (operand.opVal.operationCode == LN) {
    strcpy(*string, "ln");
    (*string) += 2;
  } else if (operand.opVal.operationCode == LOG) {
    strcpy(*string, "log");
    (*string) += 3;
  } else if (operand.opVal.operationCode == REMAINDER) {
    strcpy(*string, "mod");
    (*string) += 3;
  }
}

bool isFunction(Operand operand) {
  int code = operand.opVal.operationCode;
  if (code == COS || code == SIN || code == TAN || code == ACOS ||
      code == ASIN || code == ATAN || code == SQRT || code == LN || code == LOG)
    return true;
  return false;
}

/// @brief Compares two operands by their priority
/// @param o1 First operand
/// @param o2 Second operand
/// @return 1 - o1 is prioritized
///   -1 - o2 is prioritized
///    0 - equal priority
int comparePriority(Operand o1, Operand o2) {
  int p1 = getPriority(o1);
  int p2 = getPriority(o2);
  if (p1 < p2) return +1;
  if (p1 == p2) return 0;
  return -1;
}

int getPriority(Operand operand) {
  int code = operand.opVal.operationCode;
  if (isFunction(operand)) return 1;
  if (code == MINUS || code == PLUS) return 2;
  if (code == EXPONENTIATION) return 3;
  if (code == MULTIPLICATION || code == DIVISION || code == REMAINDER) return 4;
  return 5;  // SUMMARY || DIFFERENCE
}

Operand perform(Operand operand, Stack *stack, bool *error) {
  int code = operand.opVal.operationCode;
  Operand result = {0};
  result.isOperation = false;
  if (code == SUMMARY) {
    if (stack->size < 2) {
      *error = true;
      return result;
    }
    Operand o2 = pop(stack);
    Operand o1 = pop(stack);
    result.opVal.value = o1.opVal.value + o2.opVal.value;
  } else if (code == DIFFERENCE) {
    if (stack->size < 2) {
      *error = true;
      return result;
    }
    Operand o2 = pop(stack);
    Operand o1 = pop(stack);
    result.opVal.value = o1.opVal.value - o2.opVal.value;
  } else if (code == MULTIPLICATION) {
    if (stack->size < 2) {
      *error = true;
      return result;
    }
    Operand o2 = pop(stack);
    Operand o1 = pop(stack);
    result.opVal.value = o1.opVal.value * o2.opVal.value;
  } else if (code == DIVISION) {
    if (stack->size < 2) {
      *error = true;
      return result;
    }
    Operand o2 = pop(stack);
    Operand o1 = pop(stack);
    if (o2.opVal.value == 0.0) *error = true;
    result.opVal.value = o1.opVal.value / o2.opVal.value;
  } else if (code == EXPONENTIATION) {
    if (stack->size < 2) {
      *error = true;
      return result;
    }
    Operand o2 = pop(stack);
    Operand o1 = pop(stack);
    result.opVal.value = pow(o1.opVal.value, o2.opVal.value);
  } else if (code == REMAINDER) {
    if (stack->size < 2) {
      *error = true;
      return result;
    }
    Operand o2 = pop(stack);
    Operand o1 = pop(stack);
    result.opVal.value = fmod(o1.opVal.value, o2.opVal.value);
  } else if (code == PLUS) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    result.opVal.value = fabs(o1.opVal.value);
  } else if (code == MINUS) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    result.opVal.value = -o1.opVal.value;
  } else if (code == COS) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    result.opVal.value = cos(o1.opVal.value);
  } else if (code == SIN) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    result.opVal.value = sin(o1.opVal.value);
  } else if (code == TAN) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    if (o1.opVal.value == (3.141592 / 2)) *error = true;
    result.opVal.value = tan(o1.opVal.value);
  } else if (code == ACOS) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    if (fabs(o1.opVal.value) > 1.0) *error = true;
    result.opVal.value = acos(o1.opVal.value);
  } else if (code == ASIN) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    if (fabs(o1.opVal.value) > 1.0) *error = true;
    result.opVal.value = asin(o1.opVal.value);
  } else if (code == ATAN) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    result.opVal.value = atan(o1.opVal.value);
  } else if (code == SQRT) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    if (o1.opVal.value < 0.0) *error = true;
    result.opVal.value = sqrt(o1.opVal.value);
  } else if (code == LN) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    if (o1.opVal.value <= 0.0) *error = true;
    result.opVal.value = log(o1.opVal.value);
  } else if (code == LOG) {
    if (stack->size < 1) {
      *error = true;
      return result;
    }
    Operand o1 = pop(stack);
    if (o1.opVal.value <= 0.0) *error = true;
    result.opVal.value = log10(o1.opVal.value);
  }
  return result;
}

bool isX(Operand operand) {
  if (!operand.isOperation || operand.opVal.operationCode != VARX) return false;
  return true;
}
