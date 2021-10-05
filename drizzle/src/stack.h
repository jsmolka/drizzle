#pragma once

#include <shell/ranges.h>

template<typename T, std::size_t kSize>
class Stack
{
public:
    static_assert(kSize > 0);

    using iterator = T*;
    using const_iterator = const iterator;

    ~Stack()
    {
        if (stack != data)
            delete[] data;
    }

    void push(const T& value)
    {
        if (head == last)
            grow();
        *head++ = value;
    }

    void push(T&& value)
    {
        if (head == last)
            grow();
        *head++ = std::move(value);
    }

    T pop()
    {
        head--;
        return *head;
    }

    std::size_t capacity() const
    {
        return last - data;
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

    void shrink(std::size_t count)
    {
        head -= count;
    }

    T& peek(std::size_t index)
    {
        return *(head - index - 1);
    }

    const T& peek(std::size_t index) const
    {
        return *(head - index - 1);
    }

    T& operator[](std::size_t index)
    {
        return data[index];
    }

    const T& operator[](std::size_t index) const
    {
        return data[index];
    }

    SHELL_FORWARD_ITERATORS(data, head)

private:
    void grow()
    {
        std::size_t capacity_old = capacity();
        std::size_t capacity_new = 2 * capacity_old;

        T* data_old = data;
        T* data_new = new T[capacity_new];

        head = std::copy(begin(), end(), data_new);
        data = data_new;
        last = data_new + capacity_new;

        if (stack != data_old)
            delete[] data_old;
    }

    T* data = stack;
    T* head = stack;
    T* last = stack + kSize;
    T stack[kSize];
};
