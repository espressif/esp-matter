// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <esp_matter_mem.h>
#include <type_traits>

namespace esp_matter {

template <typename T>
class SinglyLinkedList {
public:
    /**
     * @brief Appends a node with a given value at the end of the list.
     *
     * @param head     Pointer to the head pointer of the list.
     * @param newNode  Pointer to the node to append.
     */
    static void append(T **head, T *newNode);

    /**
     * @brief Removes a specific target node from the list.
     *
     * @param head    Pointer to the head pointer of the list.
     * @param target  Pointer to the target node to remove.
     */
    static void remove(T **head, T *target);

    /**
     * @brief Deletes the entire list, freeing all nodes.
     *
     * @param head  Pointer to the head pointer of the list.
     */
    static void delete_list(T **head);

    /**
     * @brief Counts the number of nodes in the list.
     *
     * @param head  Pointer to the head node of the list.
     *
     * @return The total number of nodes in the list.
     */
    static unsigned int count(const T *head);

    /**
     * @brief Counts the number of nodes in the list that have a specific flag set.
     *
     * @param head  Pointer to the head node of the list.
     * @param flag  The flag to check in each node.
     *
     * @return The number of nodes where the specified flag is set.
     */
    static unsigned int count_with_flag(const T *head, unsigned int flag);
};

template <typename T>
void SinglyLinkedList<T>::append(T **head, T *newNode)
{
    T **p = head;
    while (*p) {
        p = &(*p)->next;
    }
    *p = newNode;
}

template <typename T>
void SinglyLinkedList<T>::remove(T **head, T *target)
{
    T **p = head;
    while (*p != target) {
        p = &(*p)->next;
    }
    *p = target->next;
    free(target);
}

template <typename T>
void SinglyLinkedList<T>::delete_list(T **head)
{
    T *current = *head;
    while (current) {
        T *next = current->next;
        free(current);
        current = next;
    }
    *head = nullptr;
}

template <typename T>
unsigned int SinglyLinkedList<T>::count(const T *head)
{
    unsigned int cnt = 0;
    while (head) {
        cnt++;
        head = head->next;
    }
    return cnt;
}

template <typename T>
unsigned int SinglyLinkedList<T>::count_with_flag(const T *head, unsigned int flag)
{
    static_assert(std::is_member_object_pointer<decltype(&T::flags)>::value, "T must have a member named 'flag'");
    unsigned int cnt = 0;
    while (head) {
        if (head->flags & flag) {
            ++cnt;
        }
        head = head->next;
    }
    return cnt;
}

} // namespace esp_matter
