/*
 * Copyright 2026, Unikie
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

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
