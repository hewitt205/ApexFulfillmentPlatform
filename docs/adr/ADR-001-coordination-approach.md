# ADR-001: Build an in-house coordination layer for M1 instead of adopting a production tool

## Status

Accepted

## Context

Apex's target architecture needs service discovery, membership, and leader election once order/inventory processing is split into independently deployable services (see `docs/COMPANY.md`, `docs/TICKETS.md` APEX-003).

Production-grade options exist (ZooKeeper, Consul, etcd, Kubernetes-native mechanisms). Any of these would solve the immediate coordination problem faster than building one.

## Decision

For M1, we are implementing a small, in-house coordination model (service registry, leases, leader election) rather than adopting one of the production tools listed above.

## Rationale

- The purpose of this milestone is to understand *why* coordination is hard — lease expiration, split-brain risk, leader-election correctness — not just to make the symptom go away by delegating to a tool.
- A minimal implementation is small enough to reason about completely, which makes it a better vehicle for the failure-engineering work planned in M5 (introducing and diagnosing real coordination failures).
- No third-party runtime dependency is introduced, which keeps the build simple during the early milestones.

## Consequences

- The in-house implementation is **not** intended to be production-hardened. It should not be presented as a replacement for etcd/Consul/ZooKeeper in a real system.
- Once the underlying concepts are demonstrated (M1) and exercised under real network conditions and failure injection (M2, M5), a follow-up ADR should evaluate whether to keep the in-house layer or migrate to a production coordination service — and should document that trade-off explicitly rather than silently swapping it out.
- This decision will need to be revisited if/when the project scope grows beyond what a single-engineer case study can reasonably maintain.

## Related

- `docs/TICKETS.md` — APEX-005
- `docs/architecture/system-context.md`
