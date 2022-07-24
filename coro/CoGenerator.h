#pragma once

#include <coroutine>

template <class ValueType>
struct CoPromiseType
{
    std::suspend_always initial_suspend()
    {
        return std::suspend_always();
    }

    std::suspend_always final_suspend() noexcept
    {
        return std::suspend_always();
    }

    void unhandled_exception()
    {
        std::terminate();
    }

    std::coroutine_handle<CoPromiseType> get_return_object()
    {
        return std::coroutine_handle<CoPromiseType>::from_promise(*this);
    }

    void return_void()
    {
    }

    std::suspend_always yield_value(const ValueType& v)
    {
        value = v;
        return std::suspend_always();
    }

    std::suspend_always yield_value(ValueType&& v)
    {
        value = std::move(v);
        return std::suspend_always();
    }

    ValueType value;
};

template <class RetType>
struct CoGenerator : public std::coroutine_handle<CoPromiseType<RetType>>
{
public:
    using promise_type = CoPromiseType<RetType>;

    CoGenerator(std::coroutine_handle<promise_type> handle)
        : std::coroutine_handle<promise_type>(handle)
    {
    }

    CoGenerator() = delete;
    CoGenerator(const CoGenerator&) = delete;
    void operator=(const CoGenerator&) = delete;

    ~CoGenerator()
    {
        this->destroy();
    }

    const RetType& Value() const
    {
        return this->promise().value;
    }

    RetType& Value()
    {
        return const_cast<RetType&>(static_cast<const CoGenerator*>(this)->Value());
    }

public:
    class iterator
    {
    public:
        iterator() = default;
        iterator(CoGenerator* g)
            : _ptr(g)
        {

        }

        RetType& operator *()
        {
            return _ptr->Value();
        }

        CoGenerator* operator->()
        {
            return _ptr;
        }

        const RetType& operator*() const
        {
            return _ptr->Value();
        }

        iterator& operator++()
        {
            _ptr->resume();
            if (_ptr->done())
            {
                _ptr = nullptr;
                return *this;
            }

            return *this;
        }

        bool operator==(const iterator& other) const = default;

    private:
        CoGenerator* _ptr = nullptr;
    };

    iterator begin()
    {
        iterator iter{ this };
        iter->resume();
        return iter;
    }

    iterator end()
    {
        return iterator{};
    }
};