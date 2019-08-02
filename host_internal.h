#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "host.h"
#include "logger.h"
#include "utils/zero_resetable.hpp"
#include "utils/macros.h"
#include <string>
#include <gcroot.h>

ref class RunspaceHolder;
ref class MyHost;

extern FreePointer FreePointerPtr;
extern AllocPointer AllocPointerPtr;

struct FreePointerHelper {
    template<typename T>
    void operator()(T* t) {
        FreePointerPtr((void*)t);
    }
};

template<typename T>
struct AutoDllFree {
    ZeroResetable<T*> t;
    DENY_COPY(AutoDllFree);
    DEFAULT_MOVE(AutoDllFree);
    AutoDllFree(T* tParam) { t = tParam; }
    AutoDllFree() :t(nullptr) {}
    void free() {

        if (t!=nullptr) {
            FreePointerPtr((void*)t.get());
            t = nullptr;
        }

    }
    T* operator->() {
        return t;
    }
    operator T* () {
        return t;
    }
    T* get() {
        return t;
    }
    ~AutoDllFree()
    {
        free();
    }
};


template<typename T>
AutoDllFree<T> MakeAutoDllFree(T* t) {
    return AutoDllFree<T>(t);
}

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
