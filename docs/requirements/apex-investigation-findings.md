# Apex — Investigation Findings

## 1. Executive problem statement

Apex's fulfillment platform has grown into a collection of systems that perform related parts of the fulfillment process but lack strong coordination between them.

The result is:

- delayed orders;
- inventory conflicts;
- poor visibility into order state;
- difficulty handling warehouse failures;
- difficult diagnosis of downstream failures;
- tightly coupled deployments;
- significant operational effort when making changes.

The existing system cannot simply be replaced. Modernization must occur alongside it.

## 2. Confirmed business problems

| Problem | Business impact |
|---|---|
| Orders slow down during peak periods | Customer dissatisfaction and delayed fulfillment |
| Inventory can be reserved inconsistently | Overselling, manual correction, customer impact |
| Warehouse outages affect fulfillment | Available inventory and fulfillment capacity change unexpectedly |
| Shipping providers can become unavailable/slow | Orders become delayed by external dependencies |
| Deployments require coordinated downtime | Changes are expensive and risky |
| No unified system visibility | Support and engineering cannot quickly determine what happened |
| Multiple systems maintain overlapping state | Different systems can disagree |
| Adding order providers is difficult | Limits business expansion |
| Failures are discovered late | Customers may report problems before operations detects them |

## 3. Investigation findings

### Inventory

Inventory is intended to be perpetual.

Warehouses maintain inventory state, but the central ordering system does not have exclusive control over all inventory-affecting operations.

This means inventory is a **business capability with multiple participants**, not simply a table belonging to the order system.

### Warehouses

A warehouse being unavailable can affect:

- available inventory;
- fulfillment capacity;
- fulfillment location;
- shipping time;
- shipping cost.

Therefore warehouse availability is part of fulfillment decision-making.

### Shipping

Shipping providers are external dependencies.

Apex cannot control their:

- availability;
- latency;
- maintenance;
- rate limits;
- APIs.

Shipping therefore needs isolation from the core order workflow where practical.

### Finance

Accounting/finance exists as a separate system.

Fulfillment depends on it for things such as:

- customer credit;
- financial authorization;
- invoice information;
- refunds/credits;
- tax information.

Finance is **outside the initial modernization boundary**. The new platform must demonstrate that it can integrate with systems it does not own.

### Order providers

Apex currently receives orders through:

- desktop application;
- web portal;
- partner API.

Additional providers are expected.

The system therefore needs a stable boundary between **order intake** and **order processing**.

### Current observability

There is no reliable end-to-end view of an order.

When something goes wrong, staff may need to investigate several systems independently.

The organization therefore has an observability problem in addition to the underlying operational problems.

## 4. Root-cause hypothesis

Our first hypothesis is:

> **Apex's major fulfillment problems are amplified by insufficient coordination, ownership, and state boundaries between systems.**

We are **not** claiming that every individual failure has the same technical root cause.

Possible contributing causes include:

- database contention;
- race conditions;
- stale state;
- latency;
- synchronous dependencies;
- infrastructure limitations;
- duplicate requests;
- poorly defined ownership;
- fragmented workflows.

**Coordination is our architectural hypothesis, not our diagnosis of every bug.**

## 5. Architectural implications

### Explicit ownership

We need to answer:

> Who owns this piece of state?

Particularly for:

- orders;
- inventory;
- warehouse availability;
- fulfillment state;
- shipping state.

### Explicit coordination

We need mechanisms for:

- service membership;
- health;
- leadership;
- work assignment;
- state transitions;
- failure detection.

This is the reason the coordination component exists.

### Asynchronous processing

Not every operation should require a synchronous chain to succeed before the customer-facing request can complete.

Queues/events will likely become important for:

- inventory processing;
- fulfillment work;
- retries;
- downstream failures.

### External dependency isolation

A shipping provider being unavailable should not automatically make the entire fulfillment platform unavailable.

The same principle applies to Finance and other legacy dependencies.

### Observability

A support employee should eventually be able to answer:

> Where is order 12345?

and:

> Why is it there?

without manually searching several systems.

The platform therefore needs:

- correlation/request IDs;
- structured logs;
- service health;
- order state;
- events;
- dependency status.

### Legacy coexistence

We cannot assume:

> "New system deployed → old system deleted."

Modernization must support gradual migration and coexistence.

Migration itself will become an engineering problem.

## 6. Functional requirements derived from the investigation

### Order processing

- accept orders from multiple providers;
- validate orders;
- process orders asynchronously where appropriate;
- track order state;
- support additional order providers.

### Inventory

- track inventory availability;
- reserve inventory safely;
- prevent conflicting reservations;
- handle concurrent requests;
- account for warehouse availability.

### Warehouse coordination

- track warehouse membership/status;
- detect unavailable warehouses;
- determine alternate fulfillment options;
- account for fulfillment trade-offs.

### Shipping

- monitor shipping-provider availability;
- isolate provider failures;
- retry appropriate failures;
- support multiple providers;
- preserve orders when shipping is temporarily unavailable.

### Coordination

- service registration;
- service discovery;
- membership;
- health;
- leader election;
- failure detection;
- work ownership.

### Observability

- trace an order across services;
- identify service failures;
- expose service health;
- expose current leaders;
- expose recent events;
- provide operational visibility.

### Operations

Eventually provide a **React operations console** showing the state of the platform and allowing appropriate operational actions.

## 7. Non-functional requirements

### Resilient

Individual service or dependency failures should not unnecessarily bring down unrelated functionality.

### Scalable

Increasing order volume should not require scaling the entire system.

### Observable

Engineers and operations should be able to determine what the system is doing.

### Maintainable

Services should have clear responsibilities and ownership.

### Extensible

Adding an order provider, warehouse, or shipping provider should not require redesigning the entire platform.

### Testable

Important behavior should be testable independently and across service boundaries.

### Incrementally deployable

The platform should support modernization without requiring a complete system replacement.

### Cost-conscious

Architecture should be justified by business requirements rather than technology fashion.

## 8. Important constraints

1. **The legacy system cannot simply be replaced.**
2. **Existing business rules have value and must be preserved.**
3. **Some systems are outside our ownership.**
4. **Finance remains outside the initial boundary.**
5. **Shipping providers are unreliable external dependencies.**
6. **Warehouses have different capabilities.**
7. **The organization has limited operational visibility.**
8. **The system must support incremental modernization.**
9. **The architecture should remain as language/stack agnostic as practical.**

The C++ implementation is an implementation choice for learning systems programming and distributed systems. The architecture should not depend on C++.

## 9. Important unknowns

We do not yet know:

- exact order volume;
- peak requests/second;
- exact inventory database behavior;
- exact transaction boundaries;
- exact warehouse protocols;
- exact shipping-provider APIs;
- exact legacy system architecture;
- precise consistency requirements for every operation;
- acceptable order-processing latency;
- recovery-time objectives;
- recovery-point objectives;
- precise cloud/on-premise deployment constraints.

We should **not invent these yet**.

As senior engineers, we will identify where these unknowns affect architecture and either investigate them or make an explicit assumption.

## 10. First architectural questions

### State ownership

> Which service owns each important piece of state?

### Boundaries

> What should actually be a service?

### Coordination

> What requires coordination, and what does not?

### Communication

> Which interactions should be synchronous APIs?

> Which should be asynchronous messages/events?

### Consistency

> Where do we require strong consistency?

> Where can we tolerate eventual consistency?

### Failure

> What happens when each dependency disappears?

### Migration

> How does the new platform coexist with the existing system?

### Operations

> What does an operator need to see and control?

## 11. Investigation conclusion

> **Investigation conclusion**
>
> Apex does not appear to have a single technical defect responsible for its fulfillment problems. The problems occur across multiple systems and dependencies.
>
> The common architectural pattern is insufficient coordination between systems that collectively participate in fulfillment.
>
> The modernization effort should therefore focus on establishing clear ownership, explicit service boundaries, coordinated workflows, resilient communication, and end-to-end observability while allowing the existing system to remain operational.
>
> This hypothesis will be tested through architectural design and subsequent implementation rather than treated as an established fact.

## 12. Transition to the next phase

The requirements investigation is now considered complete.

The apprenticeship workflow is:

1. Read the fictional company's requirements. — **Complete**
2. Identify the business and technical problems. — **Complete**
3. Determine what the system needs. — **Current phase**
4. Draw the architecture.
5. Identify the components.
6. Decide where an API belongs.
7. Decide what the UI actually needs to observe/control.
8. Record those decisions as architecture decisions.
9. **Then** build the pieces.

The next artifact will be a **System Needs / Capability Model**.

For each capability, we will ask:

> Given what Apex needs, what must the system be capable of doing?

Each capability should be challenged before it becomes an architectural component.
