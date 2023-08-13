#include <check.h>

#include "calc.h"
#include "list.h"
#include "stack.h"

START_TEST(test_stack) {
  Stack stack = stackInit();
  ck_assert_int_eq(stack.size, 0);
  ck_assert_ptr_eq(stack.top, NULL);

  int size = 10;
  Operand arr[size];
  for (int i = 0; i < size; i++) arr[i].opVal.value = (int)(rand() * INT32_MAX);
  for (int i = 0; i < size; i++) {
    push(&stack, arr[i]);
    ck_assert_int_eq(stack.size, i + 1);
  }
  for (int i = size - 1; i >= 0; i--) {
    Operand op = pop(&stack);
    ck_assert_int_eq(stack.size, i);
    ck_assert_int_eq(op.opVal.value, arr[i].opVal.value);
  }
  ck_assert_int_eq(stack.size, 0);
  ck_assert_ptr_eq(stack.top, NULL);
}
END_TEST

START_TEST(test_list) {
  List list = listInit();
  ck_assert_int_eq(list.size, 0);
  ck_assert_ptr_eq(list.first, NULL);
  ck_assert_ptr_eq(list.last, NULL);

  int size = 10;
  Operand arr1[size];
  Operand arr2[size];
  for (int i = 0; i < size; i++) {
    arr1[i].opVal.value = (int)(rand() * INT32_MAX);
    arr2[i].opVal.value = (int)(rand() * INT32_MAX);
  }
  for (int i = 0; i < size; i++) {
    addToStart(&list, arr1[i]);
    ck_assert_int_eq(list.size, i + 1);
  }
  for (int i = 0; i < size; i++) {
    addToEnd(&list, arr2[i]);
    ck_assert_int_eq(list.size, i + 1 + size);
  }
  for (int i = size - 1; i >= 0; i--) {
    Operand op = removeFromStart(&list);
    ck_assert_int_eq(op.opVal.value, arr1[i].opVal.value);
    ck_assert_int_eq(list.size, size + i);
  }
  for (int i = size - 1; i >= 0; i--) {
    Operand op = removeFromEnd(&list);
    ck_assert_int_eq(op.opVal.value, arr2[i].opVal.value);
    ck_assert_int_eq(list.size, i);
  }
  ck_assert_int_eq(list.size, 0);
  ck_assert_ptr_eq(list.first, NULL);
  ck_assert_ptr_eq(list.last, NULL);

  list = listInit();
  ck_assert_int_eq(list.size, 0);
  ck_assert_ptr_eq(list.first, NULL);
  ck_assert_ptr_eq(list.last, NULL);

  for (int i = 0; i < size; i++) {
    addToStart(&list, arr1[i]);
    ck_assert_int_eq(list.size, i + 1);
  }
  for (int i = 0; i < size; i++) {
    addToEnd(&list, arr2[i]);
    ck_assert_int_eq(list.size, i + 1 + size);
  }
  for (int i = size - 1; i >= 0; i--) {
    Operand op = removeFromStart(&list);
    ck_assert_int_eq(op.opVal.value, arr1[i].opVal.value);
    ck_assert_int_eq(list.size, size + i);
  }
  for (int i = 0; i < size; i++) {
    Operand op = removeFromStart(&list);
    ck_assert_int_eq(op.opVal.value, arr2[i].opVal.value);
    ck_assert_int_eq(list.size, size - i - 1);
  }
  ck_assert_int_eq(list.size, 0);
  ck_assert_ptr_eq(list.first, NULL);
  ck_assert_ptr_eq(list.last, NULL);

  list = listInit();
  ck_assert_int_eq(list.size, 0);
  ck_assert_ptr_eq(list.first, NULL);
  ck_assert_ptr_eq(list.last, NULL);
  for (int i = 0; i < size; i++) {
    addToStart(&list, arr1[i]);
    ck_assert_int_eq(list.size, i + 1);
  }
  listFree(&list);
  ck_assert_int_eq(list.size, 0);
  ck_assert_ptr_eq(list.first, NULL);
  ck_assert_ptr_eq(list.last, NULL);
}
END_TEST

START_TEST(test_parse_string) {
  char string[] = "1+2";
  bool error;
  List list = parseString(string, &error);
  ck_assert_int_eq(error, false);
  ck_assert_int_eq(list.size, 3);

  ck_assert_int_eq(list.first->operand.isOperation, false);
  ck_assert_double_eq(list.first->operand.opVal.value, 1.0);

  ck_assert_int_eq(list.first->next->operand.isOperation, true);
  ck_assert_double_eq(list.first->next->operand.opVal.operationCode, SUMMARY);

  ck_assert_int_eq(list.first->next->next->operand.isOperation, false);
  ck_assert_double_eq(list.first->next->next->operand.opVal.value, 2.0);

  listFree(&list);
}
END_TEST

START_TEST(test_get_as_string) {
  char string[] =
      "1.000000+2.000000*4.000000/"
      "5.000000^6.000000*7.000000+cos(sin(tan(acos(asin(atan(sqrt(ln(log(0."
      "000000)))))))))+(2.000000mod3.000000)-2.000000";
  bool error;
  List list = parseString(string, &error);
  ck_assert_int_eq(error, false);
  char *res = getAsString(list, '\0');
  ck_assert_str_eq(string, res);
  free(res);
  listFree(&list);
}
END_TEST

START_TEST(test_to_polish) {
  char string[] = "1+2+3";
  bool error;
  List infix = parseString(string, &error);
  ck_assert_int_eq(error, false);
  List polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  char *res = getAsString(polish, ' ');
  ck_assert_str_eq(res, "1.000000 2.000000 + 3.000000 +");
  listFree(&infix);
  listFree(&polish);
  free(res);
}
END_TEST

START_TEST(test_calculate) {
  char string[] = "1+2*3/4";
  bool error;
  List infix = parseString(string, &error);
  ck_assert_int_eq(error, false);
  List polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  double res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 2.5, 0.000001);

  char string1[] = "1/4+2*3";
  infix = parseString(string1, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 6.25, 0.000001);

  char string2[] = "1mod2^3";
  infix = parseString(string2, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 1.0, 0.000001);

  char string3[] = "(2 ^ 4 + 2)mod(3 * 5)";
  infix = parseString(string3, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 3.0, 0.000001);

  char string4[] = "cos(sin(2 / 5) * 6)";
  infix = parseString(string4, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, -0.6930516, 0.000001);

  char string5[] = "tan(acos(cos(asin(sin(0.5)))))";
  infix = parseString(string5, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 0.546302, 0.000001);

  char string6[] = "tan(atan(2 * 3))";
  infix = parseString(string6, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 6, 0.000001);

  char string7[] = "sqrt(ln(log(3)*5)/6)";
  infix = parseString(string7, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 0.380668, 0.000001);
}
END_TEST

START_TEST(test_calculate_errors) {
  char string[] = "5/0 * (5 - 235)";
  bool error;
  List infix = parseString(string, &error);
  ck_assert_int_eq(error, false);
  List polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string1[] = "tan(3.141592/2) * (5 - 235)";
  infix = parseString(string1, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string2[] = "acos(0-1.1) * (5 - 235)";
  infix = parseString(string2, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string3[] = "acos(1.00001) * (5 - 235)";
  infix = parseString(string3, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string4[] = "asin(1.00001) * (5 - 235)";
  infix = parseString(string4, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string5[] = "asin(0-1.000001) * (5 - 235)";
  infix = parseString(string5, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string6[] = "sqrt(0-0.0002) * (5 - 235)";
  infix = parseString(string6, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string7[] = "ln(0) * (5 - 235)";
  infix = parseString(string7, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string8[] = "ln(0-1) * (5 - 235)";
  infix = parseString(string8, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string9[] = "log(0) * (5 - 235)";
  infix = parseString(string9, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string10[] = "log(0-1234) * (5 - 235)";
  infix = parseString(string10, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);
}

START_TEST(test_calculate_unary_operations) {
  char string[] = "-2";
  bool error;
  List infix = parseString(string, &error);
  ck_assert_int_eq(error, false);
  List polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  double res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, -2.0, 0.000001);

  char string2[] = "3+(-2)";
  infix = parseString(string2, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 1.0, 0.000001);

  char string3[] = "3+-2";
  infix = parseString(string3, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 1.0, 0.000001);

  char string4[] = "3^-2";
  infix = parseString(string4, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 1.0 / 9.0, 0.000001);

  char string5[] = "-2+3";
  infix = parseString(string5, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 1, 0.000001);

  char string6[] = "+2";
  infix = parseString(string6, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 2, 0.000001);

  char string7[] = "3-(+(-2))";
  infix = parseString(string7, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculate(polish, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 1.0, 0.000001);
}
END_TEST

START_TEST(test_calculate_segfault) {
  char string[] = "-+";
  bool error;
  List infix = parseString(string, &error);
  ck_assert_int_eq(error, false);
  List polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string2[] = "3+(-2";
  infix = parseString(string2, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string3[] = "-2)";
  infix = parseString(string3, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string4[] = "+2/";
  infix = parseString(string4, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string5[] = "()";
  infix = parseString(string5, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string6[] = "";
  infix = parseString(string6, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string7[] = "?";
  infix = parseString(string7, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);

  char string8[] = "2+";
  infix = parseString(string8, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string9[] = "3-";
  infix = parseString(string9, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string10[] = "6*";
  infix = parseString(string10, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string11[] = "^3";
  infix = parseString(string11, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string12[] = "mod2";
  infix = parseString(string12, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string13[] = "+";
  infix = parseString(string13, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string14[] = "cos()";
  infix = parseString(string14, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string15[] = "sin";
  infix = parseString(string15, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string16[] = "tan()";
  infix = parseString(string16, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string17[] = "asin()";
  infix = parseString(string17, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string18[] = "acos";
  infix = parseString(string18, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string19[] = "atan";
  infix = parseString(string19, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string20[] = "-sqrt";
  infix = parseString(string20, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string21[] = "ln(((((())))))";
  infix = parseString(string21, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);

  char string22[] = "(log)";
  infix = parseString(string22, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  calculate(polish, &error);
  ck_assert_int_eq(error, true);
  listFree(&infix);
  listFree(&polish);
}
END_TEST

START_TEST(test_calculate_for_x) {
  char string[] = "sin(x)";
  bool error;
  List infix = parseString(string, &error);
  ck_assert_int_eq(error, false);
  List polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  double res = calculateForX(polish, 0.8, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, sin(0.8), 0.000001);

  char string1[] = "2+x";
  infix = parseString(string1, &error);
  ck_assert_int_eq(error, false);
  polish = toPolish(infix, &error);
  ck_assert_int_eq(error, false);
  res = calculateForX(polish, 156, &error);
  ck_assert_int_eq(error, false);
  listFree(&infix);
  listFree(&polish);
  ck_assert_double_eq_tol(res, 158.0, 0.000001);
}
END_TEST

Suite *suite_calc(void) {
  Suite *s;
  s = suite_create("calc");

  TCase *tc_structures;
  tc_structures = tcase_create("data_structures");
  tcase_add_test(tc_structures, test_stack);
  tcase_add_test(tc_structures, test_list);
  suite_add_tcase(s, tc_structures);

  TCase *tc_calc;
  tc_calc = tcase_create("calc");
  tcase_add_test(tc_calc, test_parse_string);
  tcase_add_test(tc_calc, test_get_as_string);
  tcase_add_test(tc_calc, test_to_polish);
  tcase_add_test(tc_calc, test_calculate);
  tcase_add_test(tc_calc, test_calculate_errors);
  tcase_add_test(tc_calc, test_calculate_unary_operations);
  tcase_add_test(tc_calc, test_calculate_segfault);
  tcase_add_test(tc_calc, test_calculate_for_x);
  suite_add_tcase(s, tc_calc);

  return s;
}

int main() {
  SRunner *sr;
  sr = srunner_create(suite_calc());

  srunner_run_all(sr, CK_NORMAL);

  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
