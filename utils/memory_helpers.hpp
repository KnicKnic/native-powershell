#pragma once

#include "utils/macros.hpp"
#include "utils/zero_resetable.hpp"

extern NativePowerShell_FreePointer NativePowerShell_FreePointerPtr;
extern NativePowerShell_AllocPointer NativePowerShell_AllocPointerPtr;

struct NativePowerShell_FreePointerHelper {
    template<typename T>
    void operator()(T* t) {
        NativePowerShell_FreePointerPtr((void*)t);
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
            NativePowerShell_FreePointerPtr((void*)t.get());
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

