# Apex Requirements Analysis

**Document:** Engineering Requirements Analysis  
**Source:** Apex Industrial Supply Business Requirements Brief  
**Status:** Initial analysis following engineering review

> This document records what engineering understands from the stakeholder requirements. It intentionally separates business needs from implementation choices.

---

## 1. Business Problems

### 1.1 Order fulfillment latency and throughput

Apex experiences increased order-processing delays during peak periods.

**Business impact:**

- slower fulfillment;
- reduced throughput during demand spikes;
- potential customer dissatisfaction.

The underlying concern is not simply that "the system is slow." The business needs sufficient fulfillment capacity as demand increases.

### 1.2 Inventory correctness

Multiple orders may attempt to reserve the same inventory at approximately the same time.

**Business risk:**

Apex may promise inventory to multiple customers when the inventory is not actually available.

This can result in:

- canceled orders;
- delayed orders;
- manual intervention;
- customer dissatisfaction;
- operational and financial impact.

The engineering problem is therefore to preserve inventory correctness under concurrent operations.

### 1.3 Warehouse availability

A warehouse can become unavailable because of network problems, maintenance, application failures, or warehouse-system failures.

**Business impact:**

A failure at one warehouse should not unnecessarily reduce the fulfillment capacity of the rest of the business.

The desired behavior is not necessarily perfect operation during a failure. The business may instead accept degraded capacity while continuing to fulfill orders through other warehouses.

### 1.4 External shipping dependencies

Shipping providers are outside Apex's direct operational control.

They may be slow, unavailable, return errors, or respond after a request has already been retried.

**Business problem:**

Failure or latency in one external provider should not unnecessarily prevent unrelated fulfillment work from continuing.

### 1.5 Large deployment unit

The existing application is deployed as a large unit.

**Business/operational problem:**

The unit of deployment is larger than the unit of change.

A change to one capability can require a broader deployment and interruption than the business wants.

### 1.6 Poor operational visibility

Operations currently lacks a clear view of system state and ownership.

The business needs to answer:

- what is running;
- what is healthy;
- what is failing;
- who currently owns a responsibility;
- what is processing;
- what is waiting;
- what failed;
- when it failed.

This is an operational visibility problem, not merely a request for a dashboard.

---

## 2. Functional Requirements

The eventual platform must, at minimum:

1. Provide operational visibility into platform health and activity.
2. Detect unavailable service instances.
3. Support redundant service instances.
4. Determine which instance currently owns a responsibility.
5. Support distribution of order-processing work.
6. Prevent conflicting inventory reservations.
7. Track warehouse availability.
8. Detect and isolate shipping-provider failures.
9. Support additional order sources.
10. Support addition and removal of warehouses.
11. Support addition and removal of shipping providers.
12. Allow the existing fulfillment system to operate while new capabilities are introduced.

### Requirement wording notes

Some initial ideas were deliberately reframed to avoid prescribing an implementation.

For example:

- "queues for inventory processing" became a requirement for **safe inventory operations under concurrency**.
- "services or web based" became a requirement for **independent deployment/change of capabilities**.

A queue, service, or web application may eventually be appropriate, but those are design decisions rather than requirements.

---

## 3. Non-Functional Requirements

### Reliability

A failure in one component should not unnecessarily bring down unrelated capabilities.

### Availability

The system should continue operating when individual service instances or external dependencies fail, where practical.

### Scalability

The architecture should accommodate growth in:

- order volume;
- warehouses;
- fulfillment providers;
- order sources;
- internal services.

### Observability

Operational state should be discoverable without requiring operators to log directly into individual application servers.

### Maintainability

Individual capabilities should be changeable without requiring deployment of the entire platform.

### Extensibility

New order sources, warehouses, and shipping providers should be incorporable without substantial changes to unrelated components.

### Testability

Components should be testable independently where practical.

### Documentation

The architecture, major decisions, and operational behavior should be documented.

---

## 4. Constraints

### 4.1 Incremental migration

The existing fulfillment system cannot simply be replaced.

The new platform must coexist with the existing system during modernization.

### 4.2 Potential overlapping responsibilities

During migration, old and new systems may temporarily perform overlapping work.

This creates future design questions around:

- authoritative ownership;
- duplicate processing;
- data reconciliation;
- migration sequencing;
- rollback;
- proving that migration succeeded.

These are not yet solved. They are consequences of the migration constraint.

### 4.3 Technology neutrality

No specific implementation language or framework is mandated by the business requirements.

Architecture decisions should therefore be driven by system needs rather than by selecting a technology first.

---

## 5. Future / Possible Requirements

The following capabilities were identified as likely future needs but are not yet established as immediate requirements:

- adding or removing shipping partners;
- adding or removing warehouses;
- routing work from an unavailable warehouse to another warehouse;
- communicating warehouse availability and logistics information to fulfillment;
- accepting additional order providers;
- degraded fulfillment through an alternate warehouse when the preferred warehouse is unavailable.

The warehouse-routing example exposes an important business trade-off:

> An alternate warehouse may preserve fulfillment availability while increasing fulfillment time or cost.

The platform should eventually make such trade-offs explicit rather than treating availability as a simple up/down state.

---

## 6. Open Questions

### Business scope

- Is accounting part of the fulfillment platform?
- If so, which accounting responsibilities are in scope?
- Which existing systems are authoritative for those responsibilities?

### Inventory

- Is inventory maintained as a perpetual inventory system?
- How is inventory manually audited?
- How are inventory discrepancies currently resolved?
- Which system is the source of truth for available inventory?

### Order sources

- Can warehouses currently accept orders directly?
- If they can, how are those orders represented and reconciled?
- Are all order sources expected to use the same fulfillment workflow?

### Customer and staff impact

- What are the top customer pain points?
- What are the top staff or warehouse-operator pain points?
- Which fulfillment failures are most expensive?
- Which delays are acceptable to customers?
- Which failures are unacceptable?

### Business priorities

Apex should establish relative priorities among:

- preventing inventory overselling;
- maximizing order throughput;
- minimizing fulfillment latency;
- maximizing availability;
- minimizing operational cost.

These priorities will be required when engineering must make trade-offs.

### Financial and operational constraints

- What is the expected budget for the modernization effort?
- What infrastructure costs are acceptable?
- Are cloud services permitted?
- Are on-premises systems required?
- What backup and disaster-recovery requirements exist?
- What operational staffing is available to support the new platform?

---

## 7. Important Engineering Observations

### Requirements should not prescribe solutions

The initial analysis intentionally avoids turning technical ideas into requirements.

For example:

> "We need queues."

is not yet a requirement.

A more useful requirement is:

> "Inventory operations must remain correct when multiple orders compete for the same inventory."

Engineering can later determine whether queues, transactions, locks, partitioning, or another mechanism is appropriate.

### Failure does not always mean total failure

A warehouse failure may result in slower fulfillment rather than complete service loss.

This introduces the concept of **graceful degradation** into the eventual architecture.

### Business constraints create architectural problems

The incremental migration requirement creates questions about:

- ownership;
- authority;
- duplicate processing;
- reconciliation;
- rollback.

These questions should be addressed explicitly rather than discovered accidentally during implementation.

### Architecture must remain connected to business outcomes

The goal is not simply to produce a distributed system.

The eventual architecture must demonstrate how it addresses:

- fulfillment performance;
- inventory correctness;
- failure isolation;
- operational visibility;
- independent change;
- incremental modernization.

---

## 8. Current Scope

The immediate engineering scope is intentionally narrower than the eventual platform.

The first problem to investigate is:

> **How can Apex determine which service instances exist, which responsibilities they currently own, and whether those instances are still healthy?**

No architecture or technology choice is recorded here.

Those decisions belong to the next stages of the engineering process.
