#include "list.h"

List listInit() {
  List list = {0};
  list.first = NULL;
  list.last = NULL;
  list.size = 0;

  return list;
}

void listFree(List *list) {
  while (list->size != 0) {
    removeFromStart(list);
  }
}

void addToStart(List *list, Operand operand) {
  ListNode *node = malloc(sizeof(ListNode));
  node->next = list->first;
  if (list->first != NULL)
    list->first->previous = node;
  else
    list->last = node;
  list->first = node;
  node->previous = NULL;
  node->operand = operand;
  list->size++;
}

void addToEnd(List *list, Operand operand) {
  ListNode *node = malloc(sizeof(ListNode));
  node->next = NULL;
  node->previous = list->last;
  if (list->last != NULL)
    list->last->next = node;
  else
    list->first = node;
  list->last = node;
  node->operand = operand;
  list->size++;
}

Operand removeFromStart(List *list) {
  ListNode *node = list->first;
  list->first = list->first->next;
  if (list->first != NULL) list->first->previous = NULL;
  list->size--;
  if (list->size == 0) list->last = NULL;

  Operand operand = node->operand;
  free(node);

  return operand;
}

Operand removeFromEnd(List *list) {
  ListNode *node = list->last;
  list->last = list->last->previous;
  if (list->last != NULL) list->last->next = NULL;
  list->size--;
  if (list->size == 0) list->first = NULL;

  Operand operand = node->operand;
  free(node);

  return operand;
}
