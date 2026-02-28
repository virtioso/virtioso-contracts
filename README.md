# Virtioso Shared Contracts

This repository is the SSOT for shared Virtioso RPC and trace contract headers.

Canonical include paths:

- `#include <virtioso/rpc/rpc.h>`
- `#include <virtioso/rpc/rpc_queue.h>`
- `#include <virtioso/trace/trace_contract.h>`
- `#include <virtioso/trace/trace.h>`

Rules:

- Consumers must not duplicate these contract headers elsewhere.
- Tracing callsites use `vio_trace_emit(...)` from `trace.h`.
- `trace.h` depends on `<virtioso/trace/backend.h>` and expects a backend implementation in each context.
