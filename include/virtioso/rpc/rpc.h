/*
 * Copyright 2022, 2023,, Unikie
 * Copyright 2024, Unikie
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <assert.h>
#include <inttypes.h>
#endif

#include <virtioso/rpc/rpc_queue.h>

typedef enum rpcmsg_iobuf_id {
	iobuf_id_drvrpc = 0,
	iobuf_id_devevt,
	iobuf_id_last,
} rpcmsg_iobuf_id_t;


typedef enum rpcmsg_queue_id {
	queue_id_drvrpc_req = 0,
	queue_id_drvrpc_req_dev,	/* kernel -> user */
	queue_id_drvrpc_resp,
	queue_id_devevt,
	queue_id_last,
} rpcmsg_queue_id_t;

typedef enum vso_mmio_slot_state {
	VSO_MMIO_SLOT_IDLE = 0,
	VSO_MMIO_SLOT_PENDING = 1,
	VSO_MMIO_SLOT_CLAIMED = 2,
	VSO_MMIO_SLOT_COMPLETE = 3,
} vso_mmio_slot_state_t;

#define VSO_MMIO_SLOT_COUNT RPCMSG_BUFFER_SIZE

typedef struct vso_mmio_request {
	seL4_Word addr;
	seL4_Word value;
	uint64_t generation;
	uint32_t addr_space;
	uint16_t width;
	uint8_t direction;
	uint8_t reserved0;
} vso_mmio_request_t;

typedef struct vso_mmio_completion {
	int32_t status;
	uint32_t reserved0;
	seL4_Word value;
	uint64_t generation;
} vso_mmio_completion_t;

typedef struct vso_mmio_slot {
	volatile uint32_t state;
	uint32_t reserved0;
	vso_mmio_request_t request;
	vso_mmio_completion_t completion;
} vso_mmio_slot_t;

typedef struct rpcmsg_iobuf {
	rpcmsg_buffer_t buffers[iobuf_id_last];
	rpcmsg_queue_t queues[queue_id_last];
	vso_mmio_slot_t mmio_slots[VSO_MMIO_SLOT_COUNT];
} rpcmsg_iobuf_t;

#define IOBUF_NUM_PAGES 2

#define iobuf_queue(_addr, _bid, _qid, _type)		\
	({						\
	 rpcmsg_iobuf_t *_iobuf = (void *)(_addr);	\
	 _type _q = {					\
		.buffer = &_iobuf->buffers[(_bid)],	\
		.queue = &_iobuf->queues[(_qid)],	\
	 };						\
	 _q;						\
	 })

#define driver_drvrpc_req(_addr) iobuf_queue((_addr), iobuf_id_drvrpc, queue_id_drvrpc_req, rpcmsg_rpc_queue_t)
#define driver_drvrpc_resp(_addr) iobuf_queue((_addr), iobuf_id_drvrpc, queue_id_drvrpc_resp, rpcmsg_rpc_queue_t)

#define device_km_drvrpc_req(_addr) iobuf_queue((_addr), iobuf_id_drvrpc, queue_id_drvrpc_req, rpcmsg_rpc_queue_t)
#define device_km_drvrpc_resp(_addr) iobuf_queue((_addr), iobuf_id_drvrpc, queue_id_drvrpc_resp, rpcmsg_rpc_queue_t)

#define device_drvrpc_req(_addr) iobuf_queue((_addr), iobuf_id_drvrpc, queue_id_drvrpc_req_dev, rpcmsg_rpc_queue_t)
#define device_drvrpc_resp(_addr) iobuf_queue((_addr), iobuf_id_drvrpc, queue_id_drvrpc_resp, rpcmsg_rpc_queue_t)

#define devevt_queue(_addr) iobuf_queue((_addr), iobuf_id_devevt, queue_id_devevt, rpcmsg_event_queue_t)

static_assert(sizeof(rpcmsg_iobuf_t) < 4096 * IOBUF_NUM_PAGES,
	      "Not enough of iobuf memory");

#ifndef MASK
#define MASK(_n)                        ((1UL << (_n)) - 1)
#endif

#define BIT_FIELD_MASK(_name)           (MASK(_name ## _WIDTH) << (_name ## _SHIFT))

#define BIT_FIELD_CLR_ALL(_v, _name)    ((_v) & ~BIT_FIELD_MASK(_name))
#define BIT_FIELD_SET_ALL(_v, _name)    ((_v) | BIT_FIELD_MASK(_name))

#define BIT_FIELD_GET(_v, _name)        (((_v) >> (_name ## _SHIFT)) & MASK(_name ## _WIDTH))
#define BIT_FIELD_SET(_v, _name, _n)    ((BIT_FIELD_CLR_ALL(_v, _name)) | ((((seL4_Word)(_n)) << (_name ## _SHIFT)) & BIT_FIELD_MASK(_name)))

#define RPC_MR0_OP_WIDTH                6
#define RPC_MR0_OP_SHIFT                0

/* from driver to device */
#define QEMU_OP_MMIO        0
#define QEMU_OP_PUTC_LOG    2

/* from device to driver */
#define QEMU_OP_SET_IRQ     16
#define QEMU_OP_START_VM    18
#define QEMU_OP_REGISTER_PCI_DEV    19
#define QEMU_OP_MMIO_REGION_CONFIG  20

#define QEMU_OP(_mr0)       BIT_FIELD_GET(_mr0, RPC_MR0_OP)

#define RPC_MR0_COMMON_WIDTH            (RPC_MR0_OP_WIDTH + RPC_MR0_OP_SHIFT)
#define RPC_MR0_COMMON_SHIFT            0

/************************* defines for QEMU_OP_MMIO *************************/

#define RPC_MR0_MMIO_SLOT_WIDTH         6
#define RPC_MR0_MMIO_SLOT_SHIFT         (RPC_MR0_COMMON_WIDTH + RPC_MR0_COMMON_SHIFT)

#define RPC_MR0_MMIO_DIRECTION_WIDTH    1
#define RPC_MR0_MMIO_DIRECTION_SHIFT    (RPC_MR0_MMIO_SLOT_WIDTH + RPC_MR0_MMIO_SLOT_SHIFT)

#define RPC_MR0_MMIO_DIRECTION_READ     0
#define RPC_MR0_MMIO_DIRECTION_WRITE    1

#define RPC_MR0_MMIO_ADDR_SPACE_WIDTH   8
#define RPC_MR0_MMIO_ADDR_SPACE_SHIFT   (RPC_MR0_MMIO_DIRECTION_WIDTH + RPC_MR0_MMIO_DIRECTION_SHIFT)

#define AS_GLOBAL                       MASK(RPC_MR0_MMIO_ADDR_SPACE_WIDTH)
#define AS_PCIDEV(__pcidev)             (__pcidev)

#define RPC_MR0_MMIO_LENGTH_WIDTH       4
#define RPC_MR0_MMIO_LENGTH_SHIFT       (RPC_MR0_MMIO_ADDR_SPACE_WIDTH + RPC_MR0_MMIO_ADDR_SPACE_SHIFT)

/************************* defines for QEMU_OP_SET_IRQ ***********************/
#define RPC_IRQ_CLR	0
#define RPC_IRQ_SET	1
#define RPC_IRQ_PULSE	2

/*****************************************************************************/

typedef struct vso_driver_rpc {
	rpcmsg_rpc_queue_t request;
	rpcmsg_rpc_queue_t response;
	/* used by the caller (the driver) */
	rpcmsg_buffer_state_t buffer_state;
} vso_driver_rpc_t;

typedef rpcmsg_event_queue_t vso_device_event_t;

typedef struct vso_rpc {
	/* requests from the driver to the device */
	vso_driver_rpc_t driver_rpc;

	/* requests from the device to driver */
	vso_device_event_t device_event;
	vso_mmio_slot_t *mmio_slots;

	void (*doorbell)(void *doorbell_cookie);
	void *doorbell_cookie;
} vso_rpc_t;

static inline vso_mmio_slot_t *vso_rpc_mmio_slot(vso_rpc_t *rpc, unsigned int slot)
{
	rpc_assert(rpc);
	rpc_assert(rpc->mmio_slots);
	rpc_assert(slot < VSO_MMIO_SLOT_COUNT);

	return &rpc->mmio_slots[slot];
}

static inline void vso_mmio_slots_init(vso_mmio_slot_t *slots)
{
	rpc_assert(slots);
	memset(slots, 0, sizeof(*slots) * VSO_MMIO_SLOT_COUNT);
}

#define for_each_rpc_msg(_msg, _queue)	\
	for ((_msg) = rpcmsg_receive((_queue)); (_msg); (_msg) = rpcmsg_receive((_queue)))

#define for_each_rpc_resp(_msg, _id, _buf_state, _queue)		\
	for ((_msg) = rpcmsg_receive_response((_queue), &_id);		\
	     (_msg);							\
	     rpcmsg_reclaim_buffer((_queue), (_buf_state), (_msg)),	\
	     (_msg) = rpcmsg_receive_response((_queue), &_id))

#define for_each_driver_rpc_req(_msg, _rpc)	\
	for_each_rpc_msg(_msg, &(_rpc)->driver_rpc.request)

#define for_each_driver_rpc_resp(_msg, _id, _rpc)	\
	for_each_rpc_resp((_msg), _id, (_rpc)->driver_rpc.buffer_state, &(_rpc)->driver_rpc.response)

#define for_each_device_event(_msg, _rpc)	\
	for (;!rpcmsg_event_rx(&(_rpc)->device_event, &_msg);)

static inline int vso_doorbell(vso_rpc_t *rpc)
{
	if (!rpc || !rpc->doorbell) {
		return -1;
	}

	rpc->doorbell(rpc->doorbell_cookie);

	return 0;
}

static inline int driver_rpc_request(vso_rpc_t *rpc, unsigned int op,
				     seL4_Word mr0, seL4_Word mr1,
				     seL4_Word mr2, seL4_Word mr3)
{
	int err;

	rpc_assert(rpc);

	mr0 = BIT_FIELD_SET(mr0, RPC_MR0_OP, op);

	err = rpcmsg_request(&rpc->driver_rpc.request,
			     rpc->driver_rpc.buffer_state,
			     mr0, mr1, mr2, mr3);
	if (err < 0) {
		return err;
	}

	/* FIXME: return buffer id */
	return vso_doorbell(rpc);
}

static inline int driver_rpc_request_fwd(vso_rpc_t *dst, rpcmsg_t *msg)
{
	rpc_assert(dst);
	rpc_assert(msg);

	return rpcmsg_forward(&dst->driver_rpc.request, msg);
}

static inline int driver_rpc_reply(vso_rpc_t *rpc, rpcmsg_t *msg)
{
	int err;

	rpc_assert(rpc);
	rpc_assert(msg);

	err = rpcmsg_reply(&rpc->driver_rpc.response, msg);
	if (err) {
		return err;
	}

	return vso_doorbell(rpc);
}

static inline int driver_rpc_request_pending(vso_rpc_t *rpc)
{
	rpc_assert(rpc);

	return rpcmsg_queue_empty(rpc->driver_rpc.request.queue);
}

static inline int device_event_tx(vso_rpc_t *rpc, unsigned int op,
				  seL4_Word mr0, seL4_Word mr1,
				  seL4_Word mr2, seL4_Word mr3)
{
	int err;

	mr0 = BIT_FIELD_SET(mr0, RPC_MR0_OP, op);

	err = rpcmsg_event_tx(&rpc->device_event, mr0, mr1, mr2, mr3);
	if (err) {
		return err;
	}

	return vso_doorbell(rpc);
}

/* FIXME: convert these to synchronous RPC */
static inline int device_rpc_req_start_vm(vso_rpc_t *rpc)
{
	return device_event_tx(rpc, QEMU_OP_START_VM, 0, 0, 0, 0);
}

static inline int device_rpc_req_create_vpci_device(vso_rpc_t *rpc,
						seL4_Word pcidev)
{
	return device_event_tx(rpc, QEMU_OP_REGISTER_PCI_DEV, 0, pcidev, 0, 0);
}

static inline int device_rpc_req_mmio_region_config(vso_rpc_t *rpc, uintptr_t gpa,
						size_t size,
						unsigned long flags)
{
	return device_event_tx(rpc, QEMU_OP_MMIO_REGION_CONFIG, 0, gpa, size, flags);
}

static inline int device_rpc_req_set_irqline(vso_rpc_t *rpc, seL4_Word irq)
{
	return device_event_tx(rpc, QEMU_OP_SET_IRQ, 0, irq, RPC_IRQ_SET, 0);
}

static inline int device_rpc_req_clear_irqline(vso_rpc_t *rpc, seL4_Word irq)
{
	return device_event_tx(rpc, QEMU_OP_SET_IRQ, 0, irq, RPC_IRQ_CLR, 0);
}

static inline int device_rpc_req_pulse_irqline(vso_rpc_t *rpc, seL4_Word irq)
{
	return device_event_tx(rpc, QEMU_OP_SET_IRQ, 0, irq, RPC_IRQ_PULSE, 0);
}

static inline int driver_rpc_req_mmio_start(vso_rpc_t *rpc, unsigned int direction,
					    unsigned int addr_space, unsigned int slot,
					    uint64_t generation,
					    seL4_Word addr, seL4_Word len, seL4_Word data)
{
	vso_mmio_slot_t *mmio_slot;

	rpc_assert(rpc);

	if (slot >= VSO_MMIO_SLOT_COUNT) {
		return -1;
	}

	mmio_slot = vso_rpc_mmio_slot(rpc, slot);
	if (atomic_load_acquire(&mmio_slot->state) != VSO_MMIO_SLOT_IDLE) {
		return -1;
	}

	memset(&mmio_slot->completion, 0, sizeof(mmio_slot->completion));
	mmio_slot->request.addr = addr;
	mmio_slot->request.value = data;
	mmio_slot->request.generation = generation;
	mmio_slot->request.addr_space = addr_space;
	mmio_slot->request.width = len;
	mmio_slot->request.direction = direction;
	mmio_slot->request.reserved0 = 0;

	rpcmsg_write_barrier();
	atomic_store_release(&mmio_slot->state, VSO_MMIO_SLOT_PENDING);

	return vso_doorbell(rpc);
}

static inline int driver_rpc_req_mmio_claim(vso_rpc_t *rpc, unsigned int slot,
					    vso_mmio_request_t *request)
{
	vso_mmio_slot_t *mmio_slot;
	uint32_t expected = VSO_MMIO_SLOT_PENDING;

	rpc_assert(rpc);
	rpc_assert(request);

	if (slot >= VSO_MMIO_SLOT_COUNT) {
		return -1;
	}

	mmio_slot = vso_rpc_mmio_slot(rpc, slot);
	if (!atomic_compare_and_swap(&mmio_slot->state, &expected,
				     VSO_MMIO_SLOT_CLAIMED)) {
		return -1;
	}

	rpcmsg_read_barrier();
	*request = mmio_slot->request;
	return 0;
}

static inline int driver_rpc_ack_mmio_finish(vso_rpc_t *rpc, unsigned int slot,
					     int status, uint64_t generation,
					     seL4_Word data)
{
	vso_mmio_slot_t *mmio_slot;

	rpc_assert(rpc);

	if (slot >= VSO_MMIO_SLOT_COUNT) {
		return -1;
	}

	mmio_slot = vso_rpc_mmio_slot(rpc, slot);
	if (atomic_load_acquire(&mmio_slot->state) != VSO_MMIO_SLOT_CLAIMED) {
		return -1;
	}

	mmio_slot->completion.status = status;
	mmio_slot->completion.reserved0 = 0;
	mmio_slot->completion.value = data;
	mmio_slot->completion.generation = generation;

	rpcmsg_write_barrier();
	atomic_store_release(&mmio_slot->state, VSO_MMIO_SLOT_COMPLETE);

	return vso_doorbell(rpc);
}

static inline int driver_rpc_req_mmio_requeue(vso_rpc_t *rpc, unsigned int slot,
					      uint64_t generation)
{
	vso_mmio_slot_t *mmio_slot;

	rpc_assert(rpc);

	if (slot >= VSO_MMIO_SLOT_COUNT) {
		return -1;
	}

	mmio_slot = vso_rpc_mmio_slot(rpc, slot);
	if (atomic_load_acquire(&mmio_slot->state) != VSO_MMIO_SLOT_CLAIMED ||
	    mmio_slot->request.generation != generation) {
		return -1;
	}

	atomic_store_release(&mmio_slot->state, VSO_MMIO_SLOT_PENDING);
	return 0;
}

static inline int driver_rpc_ack_mmio_consume(vso_rpc_t *rpc, unsigned int slot,
					      vso_mmio_completion_t *completion)
{
	vso_mmio_slot_t *mmio_slot;

	rpc_assert(rpc);
	rpc_assert(completion);

	if (slot >= VSO_MMIO_SLOT_COUNT) {
		return -1;
	}

	mmio_slot = vso_rpc_mmio_slot(rpc, slot);
	if (atomic_load_acquire(&mmio_slot->state) != VSO_MMIO_SLOT_COMPLETE) {
		return -1;
	}

	rpcmsg_read_barrier();
	*completion = mmio_slot->completion;
	memset(&mmio_slot->request, 0, sizeof(mmio_slot->request));
	memset(&mmio_slot->completion, 0, sizeof(mmio_slot->completion));
	atomic_store_release(&mmio_slot->state, VSO_MMIO_SLOT_IDLE);

	return 0;
}

typedef enum vso_rpc_id {
	vso_rpc_driver = 0,
	vso_rpc_device_km,
	vso_rpc_device,
} vso_rpc_id_t;

static inline int vso_driver_rpc_init(vso_rpc_id_t id, void *iobuf, vso_driver_rpc_t *drvrpc)
{
	int err = 0;

	rpc_assert(drvrpc);

	switch (id) {
	case vso_rpc_driver:
		drvrpc->request = driver_drvrpc_req(iobuf);
		drvrpc->response = driver_drvrpc_resp(iobuf);

		rpcmsg_call_queue_init(&drvrpc->request);
		rpcmsg_reply_queue_init(&drvrpc->response);
		rpcmsg_buffer_init(drvrpc->request.buffer);
		vso_mmio_slots_init(((rpcmsg_iobuf_t *)iobuf)->mmio_slots);
		break;
	case vso_rpc_device_km:
		drvrpc->request = device_km_drvrpc_req(iobuf);
		drvrpc->response = device_km_drvrpc_resp(iobuf);
		break;
	case vso_rpc_device:
		drvrpc->request = device_drvrpc_req(iobuf);
		drvrpc->response = device_drvrpc_resp(iobuf);
		break;
	default:
		err = -1;
		break;
	}

	return err;
}

static inline int vso_rpc_init(vso_rpc_t *rpc,
			       vso_rpc_id_t id,
			       void *iobuf,
			       void (*doorbell)(void *doorbell_cookie),
			       void *doorbell_cookie)
{
	if (!rpc || !doorbell) {
		return -1;
	}

	if (vso_driver_rpc_init(id, iobuf, &rpc->driver_rpc)) {
		return -1;
	}

	rpc->device_event = devevt_queue(iobuf);
	rpc->mmio_slots = ((rpcmsg_iobuf_t *)iobuf)->mmio_slots;

	rpc->doorbell = doorbell;
	rpc->doorbell_cookie = doorbell_cookie;

	/* note that we do not initialize the queues themselves -- they are
	 * initialized statically on boot and dynamically on VM reboot
	 */

	return 0;
}
