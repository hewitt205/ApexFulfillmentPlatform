# Apex System Needs — Capability Model

**Document:** System Needs / Capability Model
**Status:** Draft for Engineering Review
**Source:** apex-business-requirements.md, requirements-analysis.md, apex-investigation-findings.md, apex-modernization-strategy.md
**Workflow position:** Step 3 of 9 — "Determine what the system needs"

> This document intentionally names **capabilities**, not **components** or **mechanisms**. No technology, pattern, or implementation choice is recorded here. Those decisions belong to later steps (architecture, components, ADRs).

---

## 1. Order Intake

- Accept orders from multiple sources (desktop application, web portal, partner API, future sources).
- Keep order intake separate from order processing, so adding a new source does not require changing processing logic.

## 2. Order State Tracking

- Track the state of every order from receipt through fulfillment.
- Preserve order state across individual component failures. An order must not become unaccounted-for because one service instance died.

## 3. Inventory Correctness Under Concurrency

- Prevent two orders from reserving the same unit of inventory.
- Remain correct when multiple reservation requests occur at approximately the same time.
- This directly addresses the business's primary named risk: overselling.

## 4. Warehouse Status Awareness

- Know which warehouses are available, unavailable, or degraded.
- Know warehouse capability and capacity, not only a binary up/down state.

## 5. Fulfillment Decision-Making

- Select the best candidate warehouse for an order using factors such as distance, available stock, warehouse health, capacity, and expected fulfillment time.
- Separate the **planning** decision ("where should we try?") from the **commitment** decision ("can that warehouse actually reserve it?").

## 6. Instance Health and Responsibility Tracking

- Know which service instances currently exist and are alive.
- Know which instance currently owns a given responsibility, so work is not duplicated or dropped.
- This is the central question posed by the business requirements: how will the platform know which instances exist, what they own, and whether they are healthy.

## 7. Work Distribution

- Distribute work across available instances so no single instance becomes overwhelmed.
- Mechanism (e.g., load balancing strategy) is not decided here.

## 8. Continue-on-Failure

- Continue operating, possibly in a degraded state, when an instance or dependency fails.
- Degraded operation is an acceptable outcome; total failure is not.
- Mechanism (e.g., failover strategy) is not decided here.

## 9. External Dependency Isolation

- A shipping provider being slow or unavailable must not stop unrelated orders from processing.
- The same isolation principle applies to Finance and other systems Apex does not own.

## 10. Unified Visibility / Aggregation

- Aggregate state from source systems (orders, inventory, warehouses, shipping) into a single queryable operational picture.
- This capability feeds a future operator-facing interface, but the capability itself is aggregation and exposure of state — not the interface. The interface is a later (Step 7) decision.

## 11. Traceability

- Follow a single order across every system it touches.
- Answer "where is this order, and why" without staff manually checking multiple applications.

## 12. Independent Deployability

- Allow a single capability to be changed or deployed without redeploying the entire platform.
- Directly addresses the business complaint about coordinated, high-risk deployments.

## 13. Coexistence With Legacy

- Allow new capabilities to run alongside the existing system without disrupting it.
- Support a rollback path back to the legacy workflow during migration.

## 14. Extensibility

- Support adding a warehouse, shipping provider, or order source without requiring rework of unrelated capabilities.

---

## Capabilities Considered and Not Included

The following were mentioned in source material but are **not** included as current capabilities, because no business problem in the investigation or requirements docs currently justifies them:

- Reporting
- Pricing
- Customer notifications

These remain part of the legacy system's responsibility for now. They should only be pulled into scope if a specific business problem later requires it.

---

## Traceability Note

Every capability above maps back to a named business problem in `apex-business-requirements.md`, `requirements-analysis.md`, or `apex-investigation-findings.md`. No capability was introduced without a source problem.

---

## Next Step

Per the nine-step workflow, the next artifact is the **future-state architecture** (Step 4: draw the architecture), followed by identifying components (Step 5). No architecture or technology choice should be made until this capability list is reviewed and agreed.
