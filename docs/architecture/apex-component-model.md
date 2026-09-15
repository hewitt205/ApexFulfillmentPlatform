# Apex Coordination Platform — Component Model

**Document:** Component Identification (Layer 2 Breakdown)
**Status:** Draft for Engineering Review
**Source:** apex-future-state-architecture.md, apex-system-needs.md
**Workflow position:** Step 5 of 9 — "Identify the components"

> This document breaks the Coordination Platform (Layer 2) into individually named components, each with a single responsibility. It does not select technologies, protocols, or internal implementation details. Those decisions belong to Steps 6–8.

---

## 1. Architecture Map

```text
                         LAYER 3
                     Operator Portal
                    (built later — reads
                     from State Aggregator
                     and Event Log)
                            |
                            | (async, read)
                            v
+-------------------------------------------------------------+
|                    LAYER 2 — COORDINATION PLATFORM           |
|                                                               |
|   +----------------+        +----------------------+         |
|   | Health Registry|<------>| Assignment Tracker    |         |
|   +--------+-------+        +----------+-----------+         |
|            ^                            |                     |
|            | heartbeats                 | assigns work        |
|            |                            v                     |
|   +--------+----------+       +--------------------+          |
|   | (all components)  |       | Fulfillment Planner |          |
|   +---------------------+     +----------+-----------+         |
|                                          |                     |
|            +-----------------------------+                    |
|            | (read, async)                                    |
|            v                                                  |
|   +--------------------+                                      |
|   | State Aggregator    |                                     |
|   +--------+-----------+                                      |
|            |  (read, async, via interface)                    |
+------------|--------------------------------------------------+
             |
             v
+-------------------------------------------------------------+
|                    LAYER 1 — EXISTING SYSTEMS                |
|      Order System | Warehouse Systems | Shipping | Accounting|
+-------------------------------------------------------------+

   Sync path (commitment only):
   Fulfillment Planner ----(sync, reserve request)----> Warehouse System

   Every component also writes to:
   +--------------------+
   | Event / Trace Log   |  <- append-only, keyed by order ID
   +--------------------+
```

**Key:**
- Solid downward arrows = async, read-only, via interface
- The single sync path is explicitly called out separately — it is the one exception to "everything talks to Layer 1 through the Aggregator"
- Event Log receives writes from every component but is not queried by them; it exists for traceability, not coordination

---

## 2. Components and Responsibilities

### State Aggregator

**Responsibility:** Pull and hold a projected view of orders, inventory, and warehouse status from Layer 1 systems.

**Talks to:** Layer 1, read-only, through a defined interface — never directly against a Layer 1 database.

**Why it exists as its own component:** This is a view, not the truth. Isolating it means a slow or unavailable Layer 1 system degrades the freshness of the projection, not the availability of the whole platform. This is the direct mechanism behind the "external dependency isolation" and "continue-on-failure" needs.

**Why it's separate from Fulfillment Planner:** Pulling data and deciding what to do with that data are different responsibilities. Combining them would mean a change to warehouse-selection logic requires redeploying the data-pulling logic too — reintroducing the large-deployment-unit problem the business explicitly complained about.

---

### Health Registry

**Responsibility:** Track which service instances currently exist and are alive, based on heartbeats.

**Talks to:** Receives heartbeats from every other Layer 2 component.

**Why it exists as its own component:** This directly answers the business's central open question — "how will the platform know which instances exist and whether they're healthy" — without conflating that question with anything else.

**Why it's separate from Assignment Tracker:** "Is it alive" and "whose job is this right now" are different questions. Blurring them is the same shape of bug that caused the original inventory double-booking problem — two different states treated as one.

---

### Assignment Tracker

**Responsibility:** Track which instance currently owns a given responsibility or unit of work.

**Talks to:** Health Registry (needs to know who is alive before it can assign work to them).

**Why it exists as its own component:** Prevents duplicate work (two instances both think they own a job) and dropped work (nobody thinks they own a job). This is a genuinely new capability — nothing in Layer 1 currently does this across systems.

---

### Fulfillment Planner

**Responsibility:** Select the best candidate warehouse for an order, using the Aggregator's projected data, then perform the synchronous commitment call to reserve inventory at the warehouse.

**Talks to:** State Aggregator (async, read, for planning), Warehouse System directly (sync, for commitment).

**Why it exists as its own component:** No single warehouse system has a cross-warehouse view, so this decision cannot live in Layer 1. It is the one component that legitimately needs both the async planning view and a synchronous path to Layer 1.

**Noted assumption:** The Planner calls the warehouse directly for the commitment step, rather than routing that sync call through a separate dedicated interface component. This is the simpler default. If a narrower, dedicated sync-only interface later proves necessary (for example, to reuse the commitment path from another component, or to apply consistent retry/timeout policy in one place), that would be a revisit at the ADR stage — not a change to this component's core responsibility.

---

### Event / Trace Log

**Responsibility:** Append-only record of decisions and state changes, keyed by order ID.

**Talks to:** Receives writes from every other component. Not queried by them — it exists to serve traceability (and later, the Operator Portal), not to coordinate behavior between components.

**Why it exists as its own component:** Directly answers "where is this order and why" without requiring staff to check multiple systems by hand. Keeping it write-only from every other component's perspective avoids it becoming an accidental second coordination mechanism.

---

## 3. Boundary Rules Carried Forward

- The State Aggregator is the only component that reads from Layer 1 in the normal (async) path. The Fulfillment Planner's sync commitment call is the sole documented exception.
- No component other than the State Aggregator and Fulfillment Planner touches Layer 1 at all.
- The Event Log is a sink, not a source. No component queries it to make a decision.
- Every component is independently deployable — a change to one does not require redeploying another.

---

## 4. What This Document Does Not Decide

- Specific technologies, frameworks, or protocols for any component
- Internal data structures or storage choices
- API contracts between components
- Deployment or infrastructure topology
