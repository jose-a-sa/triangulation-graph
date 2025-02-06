#ifndef RESTORER_H
#define RESTORER_H

template<typename T>
class Restorer
{
public:
    explicit Restorer(T& object)
        : initial_state_(object),
          object_(object)

    {}

    Restorer(Restorer const&)            = delete;
    Restorer& operator=(Restorer const&) = delete;

    ~Restorer() { restore(); }

    explicit operator bool() const { return true; }
    void     operator()() const noexcept { restore(); }
    void     restore() const noexcept { object_ = initial_state_; }

private:
    T  initial_state_;
    T& object_;
};

#endif
