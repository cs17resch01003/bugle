#include "barrier_invariants_autogenerated_definitions.h"

#if defined(__1D_WORK_GROUP) || defined(__1D_THREAD_BLOCK)

#define __read_permission(T, X) \
    { \
        volatile T __read_permission_temp = X; \
        (void)__read_permission_temp; \
    }

#endif
