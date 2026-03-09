/*
 * Copyright 2026, Unikie
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __KERNEL__
#include <linux/types.h>
typedef unsigned long virtioso_backend_word_t;
#else
#include <stdint.h>
typedef uintptr_t virtioso_backend_word_t;
#endif

#include <virtioso/rpc/rpc_queue.h>

#ifndef static_assert
#define static_assert _Static_assert
#endif

#define VIRTIOSO_BACKEND_MAILBOX_MAGIC   0x56424d58u
#define VIRTIOSO_BACKEND_MAILBOX_VERSION 1u
#define VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT 32u

typedef enum virtioso_backend_slot_state {
	VIRTIOSO_BACKEND_SLOT_IDLE = 0,
	VIRTIOSO_BACKEND_SLOT_PENDING = 1,
	VIRTIOSO_BACKEND_SLOT_CLAIMED = 2,
	VIRTIOSO_BACKEND_SLOT_COMPLETE = 3,
} virtioso_backend_slot_state_t;

typedef struct virtioso_backend_request {
	virtioso_backend_word_t addr;
	virtioso_backend_word_t value;
	uint64_t generation;
	uint32_t addr_space;
	uint16_t width;
	uint8_t direction;
	uint8_t reserved0;
} virtioso_backend_request_t;

typedef struct virtioso_backend_completion {
	int32_t status;
	uint32_t reserved0;
	virtioso_backend_word_t value;
	uint64_t generation;
} virtioso_backend_completion_t;

typedef struct virtioso_backend_slot {
	volatile uint32_t state;
	uint32_t reserved0;
	virtioso_backend_request_t request;
	virtioso_backend_completion_t completion;
} virtioso_backend_slot_t;

typedef struct virtioso_backend_mailbox {
	uint32_t magic;
	uint16_t version;
	uint16_t slot_count;
	uint64_t reserved0;
	virtioso_backend_slot_t slots[VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT];
} virtioso_backend_mailbox_t;

static_assert(sizeof(virtioso_backend_mailbox_t) <= 4096,
	      "backend mailbox must fit in one page");

static inline virtioso_backend_slot_t *
virtioso_backend_mailbox_slot(virtioso_backend_mailbox_t *mailbox,
				    unsigned int slot)
{
	return &mailbox->slots[slot];
}

static inline void
virtioso_backend_mailbox_init(virtioso_backend_mailbox_t *mailbox)
{
	unsigned int slot;

	mailbox->magic = VIRTIOSO_BACKEND_MAILBOX_MAGIC;
	mailbox->version = VIRTIOSO_BACKEND_MAILBOX_VERSION;
	mailbox->slot_count = VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT;
	mailbox->reserved0 = 0;

	for (slot = 0; slot < VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT; slot++) {
		virtioso_backend_slot_t *entry =
			virtioso_backend_mailbox_slot(mailbox, slot);

		entry->state = VIRTIOSO_BACKEND_SLOT_IDLE;
		entry->reserved0 = 0;
		entry->request.addr = 0;
		entry->request.value = 0;
		entry->request.generation = 0;
		entry->request.addr_space = 0;
		entry->request.width = 0;
		entry->request.direction = 0;
		entry->request.reserved0 = 0;
		entry->completion.status = 0;
		entry->completion.reserved0 = 0;
		entry->completion.value = 0;
		entry->completion.generation = 0;
	}
}

static inline int
virtioso_backend_request_start(virtioso_backend_mailbox_t *mailbox,
			       unsigned int slot,
			       virtioso_backend_word_t addr,
			       uint64_t value,
			       uint64_t generation,
			       uint32_t addr_space,
			       uint16_t width,
			       uint8_t direction)
{
	virtioso_backend_slot_t *entry;

	if (slot >= VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT) {
		return -1;
	}

	entry = virtioso_backend_mailbox_slot(mailbox, slot);
	if (atomic_load_acquire(&entry->state) != VIRTIOSO_BACKEND_SLOT_IDLE) {
		return -1;
	}

	entry->completion.status = 0;
	entry->completion.reserved0 = 0;
	entry->completion.value = 0;
	entry->completion.generation = 0;
	entry->request.addr = addr;
	entry->request.value = value;
	entry->request.generation = generation;
	entry->request.addr_space = addr_space;
	entry->request.width = width;
	entry->request.direction = direction;
	entry->request.reserved0 = 0;

	atomic_store_release(&entry->state, VIRTIOSO_BACKEND_SLOT_PENDING);
	return 0;
}

static inline int
virtioso_backend_request_claim(virtioso_backend_mailbox_t *mailbox,
			       unsigned int slot,
			       virtioso_backend_request_t *request)
{
	virtioso_backend_slot_t *entry;
	uint32_t expected = VIRTIOSO_BACKEND_SLOT_PENDING;

	if (slot >= VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT) {
		return -1;
	}

	entry = virtioso_backend_mailbox_slot(mailbox, slot);
	if (!atomic_compare_and_swap(&entry->state, &expected,
				     VIRTIOSO_BACKEND_SLOT_CLAIMED)) {
		return -1;
	}

	*request = entry->request;
	return 0;
}

static inline int
virtioso_backend_request_requeue(virtioso_backend_mailbox_t *mailbox,
				 unsigned int slot,
				 uint64_t generation)
{
	virtioso_backend_slot_t *entry;

	if (slot >= VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT) {
		return -1;
	}

	entry = virtioso_backend_mailbox_slot(mailbox, slot);
	if (atomic_load_acquire(&entry->state) != VIRTIOSO_BACKEND_SLOT_CLAIMED ||
	    entry->request.generation != generation) {
		return -1;
	}

	atomic_store_release(&entry->state, VIRTIOSO_BACKEND_SLOT_PENDING);
	return 0;
}

static inline int
virtioso_backend_request_finish(virtioso_backend_mailbox_t *mailbox,
				unsigned int slot,
				int status,
				uint64_t generation,
				uint64_t value)
{
	virtioso_backend_slot_t *entry;

	if (slot >= VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT) {
		return -1;
	}

	entry = virtioso_backend_mailbox_slot(mailbox, slot);
	if (atomic_load_acquire(&entry->state) != VIRTIOSO_BACKEND_SLOT_CLAIMED) {
		return -1;
	}

	entry->completion.status = status;
	entry->completion.reserved0 = 0;
	entry->completion.value = value;
	entry->completion.generation = generation;

	atomic_store_release(&entry->state, VIRTIOSO_BACKEND_SLOT_COMPLETE);
	return 0;
}

static inline int
virtioso_backend_completion_consume(virtioso_backend_mailbox_t *mailbox,
				    unsigned int slot,
				    virtioso_backend_completion_t *completion)
{
	virtioso_backend_slot_t *entry;

	if (slot >= VIRTIOSO_BACKEND_MAILBOX_SLOT_COUNT) {
		return -1;
	}

	entry = virtioso_backend_mailbox_slot(mailbox, slot);
	if (atomic_load_acquire(&entry->state) != VIRTIOSO_BACKEND_SLOT_COMPLETE) {
		return -1;
	}

	*completion = entry->completion;
	entry->request.addr = 0;
	entry->request.value = 0;
	entry->request.generation = 0;
	entry->request.addr_space = 0;
	entry->request.width = 0;
	entry->request.direction = 0;
	entry->request.reserved0 = 0;
	entry->completion.status = 0;
	entry->completion.reserved0 = 0;
	entry->completion.value = 0;
	entry->completion.generation = 0;
	atomic_store_release(&entry->state, VIRTIOSO_BACKEND_SLOT_IDLE);
	return 0;
}
