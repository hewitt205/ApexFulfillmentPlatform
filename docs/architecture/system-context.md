# System Context — M1 (Coordination)

This describes the current architecture direction, scoped to what M1 actually implements. See the README for the full long-term target architecture.

## Current scope

```text
Client
  |
  v
Order API  (not yet implemented — placeholder boundary)
  |
  +----> Inventory Service   (not yet implemented — placeholder boundary)
  |
  +----> Coordination Service   <-- this milestone
  |
  +----> Event Log   (not yet implemented — placeholder boundary)
```

## What exists today (M1)

A single in-process coordination model (`src/coordination/`) providing:

- **ServiceRegistry** — tracks registered service instances and their leases.
- **Lease** — a time-bounded claim that must be renewed or the instance is considered dead.
- **LeaderElection** — given the registry's currently-alive instances, deterministically picks a leader.

These are currently exercised from a single demonstration executable (`src/main.cpp`) rather than across real processes — that boundary is introduced in M2 (Real Network Boundary).

## Why coordination first

Order processing and inventory reservation both depend on knowing:

- which service instances are alive;
- which instance currently owns a given responsibility (e.g., "which inventory-service instance is authoritative for warehouse 3 right now");
- how to detect and recover from an instance disappearing.

Building this in-process, in isolation, keeps the distributed-systems problem separable from the business-logic problem, so each can be reasoned about (and tested) on its own before they're combined.

## Explicitly out of scope for M1

- Real network transport between processes (M2).
- Order/inventory business logic (M3).
- Any UI (M6).

Diagrams in this file will be revised as later milestones land — see `docs/adr/` for the record of *why* a given revision happened.
