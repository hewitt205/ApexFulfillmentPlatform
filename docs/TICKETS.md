# Apex Engineering Work Queue

A lightweight, Git-tracked ticket log. This is not a replacement for a real issue tracker (GitHub Issues can mirror these once the repo is live) — it exists so that ticket IDs referenced in commits have a durable, versioned home.

## Conventions

- IDs are sequential: `APEX-001`, `APEX-002`, ...
- Status values: `Open`, `In Progress`, `Blocked`, `Done`.
- Type values: `requirements`, `architecture`, `feature`, `bug`, `incident`, `ops`, `dsa`, `docs`.
- Every ticket that results in code should be referenced in the relevant commit message(s), e.g. `APEX-014 implement reservation guard`.
- Closing a ticket does not require deleting it from this file — move it under "Done" with the resolution noted.

---

## Open

### APEX-003 — Define Apex system requirements
- **Type:** requirements
- **Status:** Open
- **Description:** Turn the fictional company's business background (`docs/COMPANY.md`) into an explicit set of business and technical requirements. Distinguish what's stated from what's implied. Identify ambiguities, missing information, and questions that would be asked of a real stakeholder before proceeding.
- **Definition of done:** A requirements document exists (business requirements, technical requirements, open questions, risks, explicit assumptions) and has been used as the basis for at least a first-pass architecture.

---

## Backlog (not yet started, sequenced roughly by milestone)

### APEX-004 — System context architecture
- **Type:** architecture
- **Status:** Open
- **Depends on:** APEX-003
- **Description:** Produce a system-context diagram and short narrative describing the major components (order, inventory, coordination, event log) and their boundaries, based on the requirements from APEX-003.

### APEX-005 — ADR: coordination approach
- **Type:** architecture
- **Status:** Open
- **Description:** Record the decision to build a small in-house coordination layer for M1 rather than adopting ZooKeeper/Consul/etcd/Kubernetes immediately, including the trade-offs.

### APEX-006 — Service registration
- **Type:** feature
- **Status:** Open
- **Description:** Implement registration of a service instance (id, address/metadata, initial lease) into the coordination registry.

### APEX-007 — Service leases and expiration
- **Type:** feature
- **Status:** Open
- **Description:** Implement lease expiration so that a registry entry is considered stale/dead once its lease is not renewed in time.

### APEX-008 — Leader election
- **Type:** feature
- **Status:** Open
- **Description:** Implement a simple deterministic leader-election policy over currently-alive registered instances.

### APEX-009 — Order and inventory domain objects
- **Type:** feature
- **Status:** Open
- **Description:** Define the initial `Order` and `InventoryItem` domain objects and their invariants, independent of any transport layer.

---

## Done

_(nothing closed yet — this section will grow as milestones are completed)_
