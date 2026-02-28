/*
 * Copyright 2026, Unikie
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __KERNEL__
#include <linux/vio-trace.h>

static inline void vio_trace_backend_emit(uint8_t event,
                                          uint64_t arg0,
                                          uint64_t arg1,
                                          uint64_t arg2,
                                          uint64_t arg3)
{
    vio_trace_guest_emit(VIO_TRACE_SRC_GUEST_EL1,
                         VIO_TRACE_PROD_KMOD_SEL4_VIRT,
                         event, arg0, arg1, arg2, arg3);
}
#elif defined(QEMU_OSDEP_H)
void vio_trace_backend_emit(uint8_t event,
                            uint64_t arg0,
                            uint64_t arg1,
                            uint64_t arg2,
                            uint64_t arg3);
#else
/*
 * Non-kernel/non-QEMU contexts must provide this backend symbol.
 * Missing backend is a compile/link failure by design.
 */
void vio_trace_backend_emit(uint8_t event,
                            uint64_t arg0,
                            uint64_t arg1,
                            uint64_t arg2,
                            uint64_t arg3);
#endif
