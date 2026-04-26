#ifndef _STDATOMIC_H
#define _STDATOMIC_H

typedef enum {
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST,
} memory_order;

typedef struct { unsigned char _val; } atomic_flag;
#define ATOMIC_FLAG_INIT { 0 }

#define atomic_flag_test_and_set_explicit(obj, order) \
    __atomic_test_and_set(&(obj)->_val, (order))

#define atomic_flag_clear_explicit(obj, order) \
    __atomic_clear(&(obj)->_val, (order))

#define atomic_flag_test_and_set(obj) \
    atomic_flag_test_and_set_explicit(obj, memory_order_seq_cst)

#define atomic_flag_clear(obj) \
    atomic_flag_clear_explicit(obj, memory_order_seq_cst)

typedef _Atomic(int)                atomic_int;
typedef _Atomic(unsigned int)       atomic_uint;
typedef _Atomic(long)               atomic_long;
typedef _Atomic(unsigned long)      atomic_ulong;
typedef _Atomic(long long)          atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(char)               atomic_char;
typedef _Atomic(unsigned char)      atomic_uchar;
typedef _Atomic(short)              atomic_short;
typedef _Atomic(unsigned short)     atomic_ushort;
typedef _Atomic(void *)             atomic_ptr;

#define atomic_load_explicit(obj, order)              __atomic_load_n(obj, order)
#define atomic_store_explicit(obj, val, order)        __atomic_store_n(obj, val, order)
#define atomic_exchange_explicit(obj, val, order)     __atomic_exchange_n(obj, val, order)
#define atomic_fetch_add_explicit(obj, val, order)    __atomic_fetch_add(obj, val, order)
#define atomic_fetch_sub_explicit(obj, val, order)    __atomic_fetch_sub(obj, val, order)
#define atomic_fetch_and_explicit(obj, val, order)    __atomic_fetch_and(obj, val, order)
#define atomic_fetch_or_explicit(obj, val, order)     __atomic_fetch_or(obj, val, order)
#define atomic_fetch_xor_explicit(obj, val, order)    __atomic_fetch_xor(obj, val, order)

#define atomic_compare_exchange_strong_explicit(obj, expected, desired, succ, fail) \
    __atomic_compare_exchange_n(obj, expected, desired, 0, succ, fail)

#define atomic_compare_exchange_weak_explicit(obj, expected, desired, succ, fail) \
    __atomic_compare_exchange_n(obj, expected, desired, 1, succ, fail)

#define atomic_load(obj)              atomic_load_explicit(obj, memory_order_seq_cst)
#define atomic_store(obj, val)        atomic_store_explicit(obj, val, memory_order_seq_cst)
#define atomic_exchange(obj, val)     atomic_exchange_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_add(obj, val)    atomic_fetch_add_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_sub(obj, val)    atomic_fetch_sub_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_and(obj, val)    atomic_fetch_and_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_or(obj, val)     atomic_fetch_or_explicit(obj, val, memory_order_seq_cst)
#define atomic_fetch_xor(obj, val)    atomic_fetch_xor_explicit(obj, val, memory_order_seq_cst)

#define atomic_compare_exchange_strong(obj, expected, desired) \
    atomic_compare_exchange_strong_explicit(obj, expected, desired, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_compare_exchange_weak(obj, expected, desired) \
    atomic_compare_exchange_weak_explicit(obj, expected, desired, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_thread_fence(order) __atomic_thread_fence(order)
#define atomic_signal_fence(order) __atomic_signal_fence(order)

#define ATOMIC_BOOL_LOCK_FREE     __GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE     __GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE    __GCC_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE      __GCC_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE     __GCC_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE    __GCC_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE  __GCC_ATOMIC_POINTER_LOCK_FREE

#endif