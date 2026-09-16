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
|   +--------------------+       +----------------------+         |
|   | State Aggregator    |     | Warehouse Gateway     |         |
|   +--------+-----------+     +----------+-----------+          |
|            |  (read, async, via interface)  |                  |
+------------|--------------------------------|------------------+
             |                                |
             v                                v
+-------------------------------------------------------------+
|                    LAYER 1 — EXISTING SYSTEMS                |
|      Order System | Warehouse Systems | Shipping | Accounting|
+-------------------------------------------------------------+

   Sync path (commitment only):
   Fulfillment Planner --(sync, reserve request)--> Warehouse Gateway --(sync, retry/timeout policy applied)--> Warehouse System

   Every component also writes to:
   +--------------------+
   | Event / Trace Log   |  <- append-only, keyed by order ID
   +--------------------+
```

**Key:**
- Solid downward arrows = async, read-only, via interface
- The single sync path is explicitly called out separately — it is the one exception to "everything talks to Layer 1 through the Aggregator," and it is isolated inside its own component (Warehouse Gateway) rather than handled inline by Fulfillment Planner
- Event Log receives writes from every component but is not queried by them; it exists for traceability, not coordination

---

## 2. Components and Responsibilities

### State Aggregator

**Responsibility:** Pull and hold a projected view of orders, inventory, and warehouse status from Layer 1 systems. Warehouse status includes each warehouse's operating window (when it is actually reachable/staffed), alongside distance, stock, and capacity — not only a real-time health signal. A warehouse outside its operating window is filtered out as a fulfillment candidate at planning time, before any commitment attempt is made.

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

**Responsibility:** Select the best candidate warehouse for an order, using the Aggregator's projected data (including operating windows), then hand the selected candidate to the Warehouse Gateway to attempt commitment. If commitment fails, select the next viable candidate and try again. If no candidate remains viable — for example, every warehouse capable of serving the order's region is currently outside its operating window — move the order into an "awaiting fulfillment window" state rather than continuing to retry.

**Talks to:** State Aggregator (async, read, for planning), Warehouse Gateway (sync, for commitment attempts).

**Why it exists as its own component:** No single warehouse system has a cross-warehouse view, so this decision cannot live in Layer 1. It is the component responsible for the planning decision, but it does not own the mechanics of the sync call itself — that responsibility belongs to the Warehouse Gateway (below).

**"Awaiting fulfillment window" order state:** This is a new value under the existing Order State Tracking capability, not a new capability. It exists so an order genuinely waiting on a warehouse's operating hours is visibly distinct from a stuck or failed order — preserving the traceability the platform is built to provide. What wakes a waiting order back up (scheduled re-check vs. an event fired when a warehouse's window opens) is a mechanism decision, left open for Step 6.

---

### Warehouse Gateway

**Responsibility:** Own the synchronous commitment call to a warehouse — "reserve this inventory at this warehouse" — including retry policy, timeout policy, and circuit-breaking behavior for that warehouse's connection. Reports success or failure back to the Fulfillment Planner; does not make fulfillment decisions itself.

**Talks to:** Warehouse System (sync, the sole synchronous path from Layer 2 into Layer 1). Fulfillment Planner (sync, request/response).

**Why it exists as its own component:** Warehouse connections have historically been unreliable, satellite-like links rather than always-on connections. Isolating retry/timeout/circuit-breaking logic here means that behavior is defined once, consistently, rather than reimplemented anywhere a component needs to reach a warehouse. It also keeps the Fulfillment Planner focused solely on the planning decision — it asks the Gateway to attempt a reservation and reacts to success or failure, without managing connection details itself.

---

### Event / Trace Log

**Responsibility:** Append-only record of decisions and state changes, keyed by order ID.

**Talks to:** Receives writes from every other component. Not queried by them — it exists to serve traceability (and later, the Operator Portal), not to coordinate behavior between components.

**Why it exists as its own component:** Directly answers "where is this order and why" without requiring staff to check multiple systems by hand. Keeping it write-only from every other component's perspective avoids it becoming an accidental second coordination mechanism.

---

## 3. Boundary Rules Carried Forward

- The State Aggregator is the only component that reads from Layer 1 in the normal (async) path.
- The Warehouse Gateway is the only component that performs a synchronous call into Layer 1 — this is the sole documented exception to the async-by-default rule, and it is isolated inside one component rather than spread across others.
- No component other than the State Aggregator and Warehouse Gateway touches Layer 1 at all. The Fulfillment Planner reaches Layer 1 only indirectly, through the Gateway.
- The Event Log is a sink, not a source. No component queries it to make a decision.
- Every component is independently deployable — a change to one does not require redeploying another.

---

## 4. What This Document Does Not Decide

- Specific technologies, frameworks, or protocols for any component
- Internal data structures or storage choices
- API contracts between components
- Deployment or infrastructure topology
