#pragma once

template<typename T>
class ZeroResetable {
    T var;
public:
    ZeroResetable() : var(T{ 0 }) {}
    ZeroResetable(ZeroResetable&& rhs) : var(rhs.var) {
        rhs.var = T{ 0 };
    }
    ZeroResetable& operator=(ZeroResetable&& rhs) {
        var = rhs.var;
        rhs.var = T{ 0 };
        return *this;
    }
    ZeroResetable& operator=(T& rhs) {
        var = rhs;
        return *this;
    }
    ZeroResetable& operator=(T&& rhs) {
        var = rhs;
        return *this;
    }
    ~ZeroResetable() {
        var = T{ 0 };
    }
    operator T& () { return var; }
    operator const T& ()const { return var; }
    T& get() { return var; }
    template<typename Y>
    auto operator[](Y i) {
        return var[i];
    }
    T* operator->() { return &var; }
    //auto operator*() { return *var; }
    T* operator&() { return &var; }
    const bool operator!=(T t) const { return var != t; }
};
