// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include "linked-list.h"
#include <stdlib.h>

EmberAfPluginLinkedList* emberAfPluginLinkedListInit(void)
{
  EmberAfPluginLinkedList* list =
    (EmberAfPluginLinkedList*)malloc(sizeof(EmberAfPluginLinkedList));
  if (list != NULL) {
    memset(list, 0, sizeof(EmberAfPluginLinkedList));
  }
  return list;
}

void emberAfPluginLinkedListDeinit(EmberAfPluginLinkedList* list)
{
  while (list->count > 0) {
    emberAfPluginLinkedListPopFront(list);
  }
  free(list);
}

void emberAfPluginLinkedListPushBack(
  EmberAfPluginLinkedList* list,
  void* content)
{
  EmberAfPluginLinkedListElement* element =
    (EmberAfPluginLinkedListElement*)malloc(sizeof(
                                              EmberAfPluginLinkedListElement));
  if (element != NULL) {
    element->content = content;
    element->next = NULL;
    element->previous = list->tail;
    if (list->head == NULL) {
      list->head = element;
    } else {
      list->tail->next = element;
    }
    list->tail = element;
    ++(list->count);
  }
}

void emberAfPluginLinkedListPopFront(EmberAfPluginLinkedList* list)
{
  if (list->count > 0) {
    EmberAfPluginLinkedListElement* head = list->head;
    if (list->tail == head) {
      list->tail = NULL;
    }
    list->head = list->head->next;
    free(head);
    --(list->count);
  }
}

bool emberAfPluginLinkedListRemoveElement(
  EmberAfPluginLinkedList* list,
  EmberAfPluginLinkedListElement* element)
{
  if ((element != NULL) && (list->head != NULL)) {
    if (element == list->head) {
      if (list->head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
      } else {
        list->head = element->next;
        element->next->previous = NULL;
      }
    } else if (element == list->tail) {
      list->tail = element->previous;
      element->previous->next = NULL;
    } else {
      element->previous->next = element->next;
      element->next->previous = element->previous;
    }
    --(list->count);
    free(element);
    return true;
  }
  return false;
}

bool emberAfPluginLinkedListClearAllElements(EmberAfPluginLinkedList* list)
{
  while (list->head != NULL) {
    emberAfPluginLinkedListPopFront(list);
  }
  return true;
}

EmberAfPluginLinkedListElement* emberAfPluginLinkedListNextElement(
  EmberAfPluginLinkedList* list,
  EmberAfPluginLinkedListElement* elementPosition)
{
  if (elementPosition == NULL) {
    return list->head;
  } else {
    return elementPosition->next;
  }
}
