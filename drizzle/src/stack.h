#pragma once

#include <algorithm>
#include <shell/ranges.h>

template<typename T, std::size_t N>
class Stack
{
public:
    static_assert(N > 0);

    using iterator = T*;
    using const_iterator = const iterator;

    ~Stack()
    {
        if (stack != data)
            delete[] data;
    }

    void push(const T& value)
    {
        *head = value;
        if (++head == last)
            grow();
    }

    T pop()
    {
        head--;
        return *head;
    }

    T& peek()
    {
        return *(head - 1);
    }

    std::size_t size() const
    {
        return head - data;
    }

    bool empty() const
    {
        return head == data;
    }

    void clear()
    {
        head = data;
    }

    SHELL_FORWARD_ITERATORS(data, head)

private:
    void grow()
    {
        std::size_t capacity_old = last - data;
        std::size_t capacity_new = 2 * capacity_old;

        T* data_old = data;
        T* data_new = new T[capacity_new];

        std::copy(begin(), end(), data_new);

        data = data_new;
        last = data_new + capacity_new;

        if (stack != data_old)
            delete[] data_old;
    }

    T* head = stack;
    T* last = stack + N;
    T* data = stack;
    T stack[N];
};
