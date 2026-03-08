/*
 * Copyright 2026, Unikie
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include <virtioso/trace/trace_contract.h>
#include <virtioso/trace/backend.h>

static inline void vio_trace_emit(uint8_t event,
                                  uint64_t arg0,
                                  uint64_t arg1,
                                  uint64_t arg2,
                                  uint64_t arg3)
{
    vio_trace_backend_emit(event, arg0, arg1, arg2, arg3);
}

static inline void vio_trace_mmio_begin_std(uint64_t addr,
                                            uint64_t len,
                                            uint64_t dir,
                                            uint64_t value)
{
    vio_trace_payload4_t p = vio_trace_mmio_pack_std(addr, len, dir, value);
    vio_trace_emit(VIO_TRACE_EV_MMIO_BEGIN, p.a0, p.a1, p.a2, p.a3);
}

static inline void vio_trace_mmio_end_std(uint64_t addr,
                                          uint64_t len,
                                          uint64_t dir,
                                          uint64_t value)
{
    vio_trace_payload4_t p = vio_trace_mmio_pack_std(addr, len, dir, value);
    vio_trace_emit(VIO_TRACE_EV_MMIO_END, p.a0, p.a1, p.a2, p.a3);
}

static inline void vio_trace_mmio_error_std(uint64_t addr,
                                            uint64_t len,
                                            uint64_t dir,
                                            uint64_t error_code)
{
    vio_trace_payload4_t p = vio_trace_mmio_pack_std(addr, len, dir, error_code);
    vio_trace_emit(VIO_TRACE_EV_MMIO_ERROR, p.a0, p.a1, p.a2, p.a3);
}

static inline void vio_trace_mmio_begin_qemu(uint64_t addr_space,
                                             uint64_t dir,
                                             uint64_t addr,
                                             uint64_t value)
{
    vio_trace_payload4_t p = vio_trace_mmio_pack_qemu(addr_space, dir, addr, value);
    vio_trace_emit(VIO_TRACE_EV_MMIO_BEGIN, p.a0, p.a1, p.a2, p.a3);
}

static inline void vio_trace_mmio_end_qemu(uint64_t addr_space,
                                           uint64_t dir,
                                           uint64_t addr,
                                           uint64_t value)
{
    vio_trace_payload4_t p = vio_trace_mmio_pack_qemu(addr_space, dir, addr, value);
    vio_trace_emit(VIO_TRACE_EV_MMIO_END, p.a0, p.a1, p.a2, p.a3);
}

static inline void vio_trace_mmio_error_qemu(uint64_t addr_space,
                                             uint64_t dir,
                                             uint64_t addr,
                                             uint64_t error_code)
{
    vio_trace_payload4_t p = vio_trace_mmio_pack_qemu(addr_space, dir, addr, error_code);
    vio_trace_emit(VIO_TRACE_EV_MMIO_ERROR, p.a0, p.a1, p.a2, p.a3);
}

static inline void vio_trace_mmio_slot_state(uint64_t slot,
                                             uint64_t generation,
                                             uint64_t phase_id,
                                             uint64_t aux)
{
    vio_trace_emit(VIO_TRACE_EV_MMIO_SLOT_STATE, slot, generation,
                   phase_id, aux);
}
