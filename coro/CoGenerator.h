/**
 * (c) 2023 author benyuan
 * originaly published at https://github.com/ben620/coro/blob/master/coro/CoGenerator.h
 * this is a demo usage of c++ 20 coroutine
*/
#pragma once
#include <coroutine>
#include <exception>
#include <utility>

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

    std::suspend_always return_value(ValueType&& v) noexcept
    {
        return yield_value(std::move(v));
    }

    std::suspend_always return_value(const ValueType& v) noexcept
    {
        return yield_value(v);
    }

    std::suspend_always yield_value(ValueType&& v)
    {
        value = std::move<ValueType>(v);
        return std::suspend_always();
    }

    std::suspend_always yield_value(const ValueType& v)
    {
        value = v;
        return std::suspend_always();
    }

    ValueType value;
};


template <>
struct CoPromiseType<void>
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

    void return_void() noexcept
    {
    }
};



template <class RetType>
struct CoGenerator
{
public:
    using promise_type = CoPromiseType<RetType>;

    CoGenerator(std::coroutine_handle<promise_type> handle)
        : _handle(std::move(handle))
    {
    }

    CoGenerator() = default;
    CoGenerator(const CoGenerator&) = delete;
    void operator=(const CoGenerator&) = delete;
    void operator=(CoGenerator&& r) noexcept
    {
        if (this == &r)
        {
            return;
        }
        _handle = std::move(r._handle);
        r._handle = nullptr;
    }
    CoGenerator(CoGenerator&& r) noexcept
    {
        if (this == &r)
        {
            return;
        }
        _handle = std::move(r._handle);
        r._handle = nullptr;
    }

    ~CoGenerator()
    {
        if (_handle)
        {
            _handle->destroy();
        }
    }

    const RetType& Value() const
    {
        return _handle->promise().value;
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

private:
    void resume()
    {
        if (_handle)
        {
            _handle.resume();
        }
    }

    std::coroutine_handle<CoPromiseType<RetType>> _handle;
};



template <>
struct CoGenerator<void>
{
public:
    using promise_type = CoPromiseType<void>;

    CoGenerator(std::coroutine_handle<promise_type> handle) noexcept
        : _handle(std::move(handle))
    {
    }

    CoGenerator() = default;
    CoGenerator(const CoGenerator&) = delete;
    void operator=(const CoGenerator&) = delete;
    void operator=(CoGenerator && r) noexcept
    {
        if (this == &r)
        {
            return;
        }
        _handle = std::move(r._handle);
        r._handle = nullptr;
    }
    CoGenerator(CoGenerator&& r) noexcept
    {
        if (this == &r)
        {
            return;
        }
        _handle = std::move(r._handle);
        r._handle = nullptr;
    }

    ~CoGenerator()
    {
        if (_handle)
        {
            _handle.destroy();
        }
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

        CoGenerator* operator->()
        {
            return _ptr;
        }

        iterator& operator++()
        {
            _ptr->_handle.resume();
            if (_ptr->_handle.done())
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
        iter->_handle.resume();
        return iter;
    }

    iterator end()
    {
        return iterator{};
    }

private:

    void resume()
    {
        if (_handle)
        {
            _handle.resume();
        }
    }

    std::coroutine_handle<CoPromiseType<void>> _handle;
};


