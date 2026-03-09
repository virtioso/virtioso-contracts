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
