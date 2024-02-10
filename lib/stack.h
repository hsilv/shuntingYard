#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stdexcept>
#include <algorithm>

template <typename T>
class Stack
{
private:
    T *top;
    int size;
    std::vector<T> elements;

public:
    Stack() : top(nullptr), size(0) {}
    void push(T const &value);
    bool isEmpty();
    T pop();
    T *getTop();
    T last();
    int getSize();
    void reverse();
};

#include "stack.h"

template <typename T>
void Stack<T>::push(T const &value)
{
    elements.push_back(value);
    size++;
    top = &elements[size - 1];
}

template <typename T>
bool Stack<T>::isEmpty()
{
    return size == 0;
}

template <typename T>
T Stack<T>::pop()
{
    if (isEmpty())
    {
        throw std::out_of_range("Mismatched operators");
    }
    else
    {
        size--;
        T value = elements[size];
        elements.pop_back();
        if (size > 0)
        {
            top = &elements[size - 1];
        }
        else
        {
            top = nullptr;
        }
        return value;
    }
}

template <typename T>
T *Stack<T>::getTop()
{
    return top;
}

template <typename T>
T Stack<T>::last()
{
    return elements[size - 1];
}

template <typename T>
int Stack<T>::getSize()
{
    return size;
}

template <typename T>
void Stack<T>::reverse()
{
    std::reverse(elements.begin(), elements.end());
}

#endif