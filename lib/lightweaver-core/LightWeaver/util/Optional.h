#pragma once

template<typename T>
class Optional {
    private:
        T* data;
    public:  
        static Optional<T> of(T& obj) {
            data = &new T(obj);
        };
        static Optional<T> empty() {};

};