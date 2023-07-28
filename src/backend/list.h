#include <stdlib.h>

#include "operand.h"

#ifndef LIST
#define LIST

/// @brief Structure, representing a List's node (holds an Operand in it)
typedef struct ListNode {
  struct ListNode *next;
  struct ListNode *previous;
  struct Operand operand;
} ListNode;

/// @brief Structure, representing a list data structure
///     (holds all the data, necessary to perform operations with list)
typedef struct List {
  ListNode *first;
  ListNode *last;
  int size;
} List;

/// @brief Initialize a List
/// @return List structure
List listInit();

/// @brief Removes all the elements from List
/// @param list List structure
void listFree(List *list);

/// @brief Add Operand to List's beginning
/// @param list List structure
/// @param operand Operand to add
void addToStart(List *list, Operand operand);

/// @brief Add Operand to List's end
/// @param list List structure
/// @param operand Operand to add
void addToEnd(List *list, Operand operand);

/// @brief Remove Operand from List's beginning
/// @param list List struture
/// @return Removed Operand
Operand removeFromStart(List *list);

/// @brief Remove Operand from List's end
/// @param list List struture
/// @return Removed Operand
Operand removeFromEnd(List *list);

#endif
