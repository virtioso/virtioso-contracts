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

typedef enum vio_trace_source_id {
    VIO_TRACE_SRC_GUEST_EL1 = 6,
    VIO_TRACE_SRC_GUEST_EL0 = 7,
    VIO_TRACE_SRC_VMM = 8,
} vio_trace_source_id_t;

typedef enum vio_trace_producer_id {
    VIO_TRACE_PROD_VIRTIO_CONSOLE = 1,
    VIO_TRACE_PROD_CAMKES_VMM = 2,
    VIO_TRACE_PROD_KMOD_SEL4_VIRT = 3,
    VIO_TRACE_PROD_QEMU_SEL4_ACCEL = 4,
    VIO_TRACE_PROD_GUEST_KERNEL = 5,
} vio_trace_producer_id_t;

/* Initial registry from docs/plans/vm1-virtio-console-cross-el-tracepoints-plan.md */
typedef enum vio_trace_event_id {
    VIO_TRACE_EV_PROBE_ENTER = 1,
    VIO_TRACE_EV_PROBE_EXIT = 2,
    VIO_TRACE_EV_PROBE_ERROR_EXIT = 3,
    VIO_TRACE_EV_MULTIPORT_DECISION = 4,
    VIO_TRACE_EV_DEVICE_READY_SEND = 5,
    VIO_TRACE_EV_EARLY_WAIT_ENTER = 6,
    VIO_TRACE_EV_EARLY_WAIT_EXIT = 7,
    VIO_TRACE_EV_IRQ_SET = 8,
    VIO_TRACE_EV_IRQ_CLEAR = 9,
    VIO_TRACE_EV_RPC_REQ = 10,
    VIO_TRACE_EV_RPC_RESP = 11,
    VIO_TRACE_EV_WAIT_IO_ENTER = 12,
    VIO_TRACE_EV_WAIT_IO_WAKE = 13,
    VIO_TRACE_EV_MMIO_BEGIN = 14,
    VIO_TRACE_EV_MMIO_END = 15,
    VIO_TRACE_EV_MMIO_ERROR = 16,
    VIO_TRACE_EV_PCI_PROBE_BEGIN = 17,
    VIO_TRACE_EV_PCI_PROBE_END = 18,
    VIO_TRACE_EV_PCI_PROBE_ERROR = 19,
    VIO_TRACE_EV_VM_CREATE_BEGIN = 20,
    VIO_TRACE_EV_VM_CREATE_END = 21,
    VIO_TRACE_EV_VM_CREATE_ERROR = 22,
    VIO_TRACE_EV_VIRTIO_CONSOLE_INIT_ENTER = 23,
    VIO_TRACE_EV_VIRTIO_CONSOLE_INIT_EXIT = 24,
    VIO_TRACE_EV_VIRTIO_CONSOLE_INIT_ERROR = 25,
    VIO_TRACE_EV_VM0_QEMU_LAUNCH = 26,
    VIO_TRACE_EV_VM0_QEMU_HEARTBEAT = 27,
    VIO_TRACE_EV_VM0_QEMU_EXIT = 28,
    VIO_TRACE_EV_VIRTIO_CONSOLE_CLASS_READY = 29,
    VIO_TRACE_EV_VIRTIO_CONSOLE_DRIVER_REG_BEGIN = 30,
    VIO_TRACE_EV_VIRTIO_CONSOLE_DRIVER_REG_END = 31,
    /* Virtio-console probe progress marker:
     * a0=step_id, a1=detail0, a2=detail1, a3=rc_or_aux. */
    VIO_TRACE_EV_VIRTIO_CONSOLE_PROBE_STEP = 32,
    /* Generic guest IRQ receive marker:
     * a0=hwirq, a1=linux_irq, a2=reserved, a3=reserved. */
    VIO_TRACE_EV_GUEST_IRQ_RECEIVE = 33,
    /* RPC ring doorbell marker emitted immediately before vso_doorbell():
     * a0=op, a1=mr1, a2=mr2, a3=mr3. */
    VIO_TRACE_EV_RING_DOORBELL = 34,
    /* libsel4vm fault-advance marker:
     * a0=vcpu_id, a1=fault_addr, a2=fault_ip, a3=0. */
    VIO_TRACE_EV_ADVANCE_VCPU_FAULT = 35,
    /* libsel4vm fault-restart marker:
     * a0=vcpu_id, a1=fault_addr, a2=fault_ip, a3=0. */
    VIO_TRACE_EV_RESTART_VCPU_FAULT = 36,
    /* kmod userspace-forward marker:
     * a0=op, a1=mr1, a2=mr2, a3=mr3. */
    VIO_TRACE_EV_RPC_FWD = 37,
} vio_trace_event_id_t;

typedef struct vio_trace_payload4 {
    uint64_t a0;
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
} vio_trace_payload4_t;

/* MMIO payload layout for guest-el1 and VMM producers:
 * a0=addr, a1=len, a2=dir, a3=value_or_error */
static inline vio_trace_payload4_t vio_trace_mmio_pack_std(uint64_t addr,
                                                           uint64_t len,
                                                           uint64_t dir,
                                                           uint64_t value_or_error)
{
    return (vio_trace_payload4_t){
        .a0 = addr,
        .a1 = len,
        .a2 = dir,
        .a3 = value_or_error,
    };
}

/* MMIO payload layout for QEMU guest-el0 producer:
 * a0=addr_space, a1=dir, a2=addr, a3=value_or_error */
static inline vio_trace_payload4_t vio_trace_mmio_pack_qemu(uint64_t addr_space,
                                                            uint64_t dir,
                                                            uint64_t addr,
                                                            uint64_t value_or_error)
{
    return (vio_trace_payload4_t){
        .a0 = addr_space,
        .a1 = dir,
        .a2 = addr,
        .a3 = value_or_error,
    };
}

static inline uint16_t vio_trace_pack_src_prod(vio_trace_source_id_t source,
                                               vio_trace_producer_id_t producer)
{
    return (((uint16_t)source & 0xFFu) << 8) | ((uint16_t)producer & 0xFFu);
}
