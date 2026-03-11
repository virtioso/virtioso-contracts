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
    /* MMIO slot lifecycle marker:
     * a0=slot, a1=mmio_generation, a2=phase_id, a3=aux. */
    VIO_TRACE_EV_MMIO_SLOT_STATE = 37,
    /* Backend mailbox/vmfd MMIO lifecycle marker:
     * a0=phase_id, a1=generation_or_token, a2=arg0, a3=arg1. */
    VIO_TRACE_EV_BACKEND_MMIO_STATE = 38,
} vio_trace_event_id_t;

typedef enum vio_trace_mmio_slot_phase_id {
    VIO_TRACE_MMIO_SLOT_CLAIM = 1,
    VIO_TRACE_MMIO_SLOT_DELEGATE = 2,
    VIO_TRACE_MMIO_SLOT_REQUEUE = 3,
    VIO_TRACE_MMIO_SLOT_COMPLETE = 4,
} vio_trace_mmio_slot_phase_id_t;

typedef enum vio_trace_backend_mmio_phase_id {
    VIO_TRACE_BACKEND_MMIO_ASYNC_PUBLISH = 1,
    VIO_TRACE_BACKEND_MMIO_BLOCKING_PUBLISH = 2,
    VIO_TRACE_BACKEND_MMIO_KMOD_CLAIM = 3,
    VIO_TRACE_BACKEND_MMIO_KMOD_DELEGATE = 4,
    VIO_TRACE_BACKEND_MMIO_KMOD_REQUEUE = 5,
    VIO_TRACE_BACKEND_MMIO_QEMU_RECV = 6,
    VIO_TRACE_BACKEND_MMIO_QEMU_COMPLETE = 7,
    VIO_TRACE_BACKEND_MMIO_KMOD_COMPLETE = 8,
    VIO_TRACE_BACKEND_MMIO_VMM_DRAIN = 9,
    VIO_TRACE_BACKEND_MMIO_VMM_RESPOND = 10,
    VIO_TRACE_BACKEND_MMIO_BLOCKING_WAIT_ENTER = 11,
    VIO_TRACE_BACKEND_MMIO_BLOCKING_WAIT_WAKE = 12,
    VIO_TRACE_BACKEND_MMIO_PUBLISH_ERROR = 13,
    VIO_TRACE_BACKEND_MMIO_COMPLETE_ERROR = 14,
    /* Shared virtqueue/ioeventfd checkpoints:
     * a1=queue index or token, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_QEMU_VIRTQUEUE_KICK = 15,
    VIO_TRACE_BACKEND_MMIO_QEMU_HOST_NOTIFIER = 16,
    VIO_TRACE_BACKEND_MMIO_QEMU_NOTIFY_VQ = 17,
    VIO_TRACE_BACKEND_MMIO_QEMU_GUEST_NOTIFY = 18,
    /* Virtio-blk checkpoints:
     * a1=queue index or status, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_QEMU_BLK_HANDLE_OUTPUT = 19,
    VIO_TRACE_BACKEND_MMIO_QEMU_BLK_HANDLE_REQUEST = 20,
    VIO_TRACE_BACKEND_MMIO_QEMU_BLK_REQ_COMPLETE = 21,
    /* Virtio-serial checkpoints:
     * a1=queue index or event, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_QEMU_SERIAL_HANDLE_OUTPUT = 22,
    VIO_TRACE_BACKEND_MMIO_QEMU_SERIAL_CONTROL_OUT = 23,
    VIO_TRACE_BACKEND_MMIO_QEMU_SERIAL_CONTROL_MSG = 24,
    /* Guest split virtqueue checkpoints:
     * a1=queue index, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_GUEST_VQ_SETUP = 25,
    VIO_TRACE_BACKEND_MMIO_GUEST_ADD_BUF = 26,
    VIO_TRACE_BACKEND_MMIO_GUEST_AVAIL_PUBLISH = 27,
    VIO_TRACE_BACKEND_MMIO_GUEST_KICK_PREPARE = 28,
    VIO_TRACE_BACKEND_MMIO_GUEST_NOTIFY = 29,
    /* Host split virtqueue visibility checkpoints:
     * a1=queue index, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_QEMU_VQ_SETUP = 30,
    VIO_TRACE_BACKEND_MMIO_QEMU_SPLIT_EMPTY = 31,
    /* Shared-memory mapping checkpoints:
     * a1=region_or_queue_id, a2=addr_or_ptr, a3=packed size/attr detail. */
    VIO_TRACE_BACKEND_MMIO_KMOD_MEM_MAP_SET = 32,
    VIO_TRACE_BACKEND_MMIO_QEMU_RAM_MAP = 33,
    VIO_TRACE_BACKEND_MMIO_VMM_RAM_DATAPORT_SETUP = 34,
    VIO_TRACE_BACKEND_MMIO_VMM_RAM_DATAPORT_MAP = 35,
    VIO_TRACE_BACKEND_MMIO_LIBSEL4VM_RESERVE_MEMORY = 36,
    VIO_TRACE_BACKEND_MMIO_LIBSEL4VM_MAP_RESERVATION = 37,
    VIO_TRACE_BACKEND_MMIO_QEMU_REGION_CACHE_DESC = 38,
    VIO_TRACE_BACKEND_MMIO_QEMU_REGION_CACHE_AVAIL = 39,
    VIO_TRACE_BACKEND_MMIO_QEMU_REGION_CACHE_USED = 40,
    /* Guest virtio-pci modern probe/setup checkpoints:
     * a1=queue/cap index, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_FIND_VQS = 41,
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_SETUP_VQ = 42,
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_MODERN_COMMON = 43,
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_MODERN_ISR = 44,
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_MODERN_NOTIFY = 45,
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_MODERN_DEVICE = 46,
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_QUEUE_ADDRESS = 47,
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_QUEUE_ENABLE = 48,
    VIO_TRACE_BACKEND_MMIO_GUEST_PCI_NOTIFY_MAP = 49,
    /* Host avail-ring cache/read checkpoints:
     * a1=queue index, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_QEMU_REGION_CACHE_AVAIL_META = 50,
    VIO_TRACE_BACKEND_MMIO_QEMU_AVAIL_READ = 51,
    /* Host cached-physmem slow-path checkpoints:
     * a1=cache token, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_QEMU_CACHE_TRANSLATE = 52,
    VIO_TRACE_BACKEND_MMIO_QEMU_CACHE_READ_SLOW = 53,
    /* Host cached IOMMU translation checkpoints:
     * a1=cache or region token, a2=aux0, a3=aux1. */
    VIO_TRACE_BACKEND_MMIO_QEMU_CACHE_IOMMU_START = 54,
    VIO_TRACE_BACKEND_MMIO_QEMU_CACHE_IOTLB = 55,
    VIO_TRACE_BACKEND_MMIO_QEMU_CACHE_SECTION = 56,
    /* Host cache-init checkpoints:
     * a1=cache token, a2=mr or offset, a3=packed meta. */
    VIO_TRACE_BACKEND_MMIO_QEMU_CACHE_INIT = 57,
    VIO_TRACE_BACKEND_MMIO_QEMU_CACHE_INIT_SECTION = 58,
} vio_trace_backend_mmio_phase_id_t;

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
