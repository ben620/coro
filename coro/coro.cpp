#include <coroutine>
#include <iostream>

template <class ValueType>
struct PromiseType
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

    std::coroutine_handle<PromiseType> get_return_object()
    {
        return std::coroutine_handle<PromiseType>::from_promise(*this);
    }

    void return_void()
    {
    }

    std::suspend_always yield_value(const ValueType &v)
    {
        value = v;
        return std::suspend_always();
    }

    std::suspend_always yield_value(ValueType && v)
    {
        value = std::move(v);
        return std::suspend_always();
    }

    ValueType value;
};

template <class RetType>
struct Generator : public std::coroutine_handle<PromiseType<RetType>>
{
public:
    using promise_type = PromiseType<RetType>;

    Generator(std::coroutine_handle<promise_type> handle)
        : std::coroutine_handle<promise_type>(handle)
    {
    }

    Generator() = delete;
    Generator(const Generator&) = delete;
    void operator=(const Generator&) = delete;

    ~Generator()
    {
        this->destroy();
    }

    const RetType& Value() const
    {
        return this->promise().value;
    }

    RetType& Value()
    {
        return const_cast<RetType &>(static_cast<const Generator*>(this)->Value());
    }

public:
    class iterator
    {
    public:
        iterator() = default;
        iterator(Generator* g)
            : _ptr(g)
        {

        }

        RetType& operator *()
        {
            return _ptr->Value();
        }

        Generator* operator->()
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
        Generator* _ptr = nullptr;
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


Generator<int> Test()
{
    co_yield 1;
    co_yield 2;
    co_yield 3;
}


int main()
{
    Generator gen = Test();
    for (int ii : gen)
    {
        std::cout << ii << std::endl;
    }

    return 0;
}
