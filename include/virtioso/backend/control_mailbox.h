/*
 * Copyright 2026, Unikie
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <virtioso/rpc/rpc_queue.h>

#define VIRTIOSO_CONTROL_MAILBOX_MAGIC      0x56434d58u
#define VIRTIOSO_CONTROL_MAILBOX_VERSION    1u
#define VIRTIOSO_CONTROL_MAILBOX_SLOT_COUNT 16u

typedef enum virtioso_control_slot_state {
	VIRTIOSO_CONTROL_SLOT_IDLE = 0,
	VIRTIOSO_CONTROL_SLOT_PENDING = 1,
	VIRTIOSO_CONTROL_SLOT_CLAIMED = 2,
} virtioso_control_slot_state_t;

typedef struct virtioso_control_event {
	uint32_t op;
	uint32_t reserved0;
	virtioso_backend_word_t mr1;
	virtioso_backend_word_t mr2;
	virtioso_backend_word_t mr3;
} virtioso_control_event_t;

typedef struct virtioso_control_slot {
	volatile uint32_t state;
	uint32_t reserved0;
	virtioso_control_event_t event;
} virtioso_control_slot_t;

typedef struct virtioso_control_mailbox {
	uint32_t magic;
	uint16_t version;
	uint16_t slot_count;
	uint64_t reserved0;
	virtioso_control_slot_t slots[VIRTIOSO_CONTROL_MAILBOX_SLOT_COUNT];
} virtioso_control_mailbox_t;

static_assert(sizeof(virtioso_control_mailbox_t) <= 4096,
	      "control mailbox must fit in one page");

static inline virtioso_control_slot_t *
virtioso_control_mailbox_slot(virtioso_control_mailbox_t *mailbox,
			      unsigned int slot)
{
	return &mailbox->slots[slot];
}

static inline void
virtioso_control_mailbox_init(virtioso_control_mailbox_t *mailbox)
{
	unsigned int slot;

	mailbox->magic = VIRTIOSO_CONTROL_MAILBOX_MAGIC;
	mailbox->version = VIRTIOSO_CONTROL_MAILBOX_VERSION;
	mailbox->slot_count = VIRTIOSO_CONTROL_MAILBOX_SLOT_COUNT;
	mailbox->reserved0 = 0;

	for (slot = 0; slot < VIRTIOSO_CONTROL_MAILBOX_SLOT_COUNT; slot++) {
		virtioso_control_slot_t *entry =
			virtioso_control_mailbox_slot(mailbox, slot);

		entry->state = VIRTIOSO_CONTROL_SLOT_IDLE;
		entry->reserved0 = 0;
		memset(&entry->event, 0, sizeof(entry->event));
	}
}

static inline int
virtioso_control_event_publish(virtioso_control_mailbox_t *mailbox,
			       uint32_t op,
			       virtioso_backend_word_t mr1,
			       virtioso_backend_word_t mr2,
			       virtioso_backend_word_t mr3)
{
	unsigned int slot;

	for (slot = 0; slot < VIRTIOSO_CONTROL_MAILBOX_SLOT_COUNT; slot++) {
		virtioso_control_slot_t *entry =
			virtioso_control_mailbox_slot(mailbox, slot);
		uint32_t expected = VIRTIOSO_CONTROL_SLOT_IDLE;

		if (!atomic_compare_and_swap(&entry->state, &expected,
					     VIRTIOSO_CONTROL_SLOT_CLAIMED)) {
			continue;
		}

		entry->event.op = op;
		entry->event.reserved0 = 0;
		entry->event.mr1 = mr1;
		entry->event.mr2 = mr2;
		entry->event.mr3 = mr3;
		rpcmsg_write_barrier();
		atomic_store_release(&entry->state, VIRTIOSO_CONTROL_SLOT_PENDING);
		return 0;
	}

	return -1;
}

static inline int
virtioso_control_event_consume(virtioso_control_mailbox_t *mailbox,
			       unsigned int slot,
			       virtioso_control_event_t *event)
{
	virtioso_control_slot_t *entry;
	uint32_t expected = VIRTIOSO_CONTROL_SLOT_PENDING;

	if (slot >= VIRTIOSO_CONTROL_MAILBOX_SLOT_COUNT) {
		return -1;
	}

	entry = virtioso_control_mailbox_slot(mailbox, slot);
	if (!atomic_compare_and_swap(&entry->state, &expected,
				     VIRTIOSO_CONTROL_SLOT_CLAIMED)) {
		return -1;
	}

	rpcmsg_read_barrier();
	*event = entry->event;
	memset(&entry->event, 0, sizeof(entry->event));
	atomic_store_release(&entry->state, VIRTIOSO_CONTROL_SLOT_IDLE);
	return 0;
}
