#pragma once
#include <gcroot.h>

template<typename T>
struct AutoDispose {
    gcroot<T> t;
    bool freed = false;
    AutoDispose(const AutoDispose& obj) = delete;
    AutoDispose(T tParam) :t(tParam) {}
    AutoDispose() :freed(true) {}
    AutoDispose(AutoDispose&& rhs)
    {
        t = rhs.t;
        rhs.freed = true;
    }
    AutoDispose& operator=(AutoDispose&& p)
    {
        t = rhs.t;
        rhs.freed = true;
    }
    AutoDispose& operator=(AutoDispose& obj) = delete;
    void free() {

        if (!freed) {
            T toDelete = t;
            delete toDelete;
            freed = true;
        }

    }
    T operator->() {
        return t;
    }
    ~AutoDispose()
    {
        free();
    }
};

template<typename T>
AutoDispose<T> MakeUsing(T t) {
    return AutoDispose<T>(t);
}