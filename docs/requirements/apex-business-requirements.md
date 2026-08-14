# Apex Industrial Supply
## Fulfillment Platform Modernization — Business Requirements

**Document:** Business Requirements Brief  
**Status:** Draft for Engineering Review  
**Stakeholder:** Operations and Fulfillment Leadership

---

## 1. Business Background

Apex Industrial Supply sells industrial parts to manufacturing companies throughout North America.

Customers place orders through:

- the Apex desktop application;
- the Apex web portal;
- partner integrations.

Apex currently operates a large application that handles most fulfillment operations.

The system has grown over approximately 15 years.

It currently handles:

- customer orders;
- inventory;
- warehouse operations;
- shipping;
- pricing;
- customer notifications;
- reporting;
- integrations with external partners.

The system is reliable enough for normal business operations, but growth has exposed several problems.

Apex expects order volume to continue increasing.

The company also expects to add additional warehouses and third-party fulfillment partners.

---

## 2. Current Problems

### Order processing

Orders can take significantly longer to process during peak periods.

A delay in one part of the fulfillment process can cause delays elsewhere.

### Inventory

Inventory availability is shared across multiple parts of the system.

Operations has observed situations where two orders attempt to reserve the same inventory at approximately the same time.

The business needs to prevent inventory from being incorrectly oversold.

### Warehouse operations

Apex currently operates multiple warehouses.

A warehouse may be temporarily unavailable because of:

- network problems;
- maintenance;
- application failures;
- warehouse system failures.

The rest of the fulfillment platform should continue operating when one warehouse becomes unavailable.

### Shipping

Shipping providers are external systems.

A shipping provider may:

- respond slowly;
- become unavailable;
- return errors;
- accept a request but respond late.

A shipping failure should not prevent unrelated orders from being processed.

### Deployments

The existing application is deployed as a large unit.

A change to one area can require deployment of the entire application.

Operations would like engineering teams to be able to deploy portions of the platform independently.

### Ownership

There is currently no clear operational ownership of individual application responsibilities.

When something fails, determining which component is responsible can require investigation across the entire application.

---

## 3. Operational Requirements

The new platform should provide operations with enough information to answer questions such as:

- Which services are currently running?
- Which warehouses are available?
- Which instance is currently responsible for a particular responsibility?
- When did a service last communicate with the platform?
- What services are experiencing failures?
- Which orders are currently being processed?
- Which orders are waiting for inventory?
- Which orders have encountered errors?

Operations should not need to log directly into individual application servers to answer these questions.

---

## 4. Reliability Requirements

The platform should tolerate individual component failures where practical.

Examples:

- If one instance of a service fails, another instance should be able to continue the responsibility.
- If a shipping provider becomes unavailable, orders that do not require that provider should continue processing.
- If a warehouse becomes unavailable, other warehouses should continue operating.

The system must avoid creating duplicate fulfillment actions when requests are retried.

---

## 5. Growth Requirements

Apex expects the platform to eventually support:

- additional warehouses;
- additional fulfillment providers;
- additional order sources;
- increased order volume;
- additional internal services.

Engineering does not want every new integration to require modification of one enormous application.

---

## 6. Technology Requirements

There is **no mandated implementation language or framework** for the new platform.

The engineering organization wants the architecture to avoid unnecessary dependence on a particular vendor or technology.

The platform should expose well-defined interfaces between major components.

The first implementation will be used as a **technical prototype**.

It does not need to replace the existing production system immediately.

---

## 7. Migration Requirement

The existing fulfillment system cannot simply be turned off and replaced.

Apex expects modernization to happen incrementally.

The existing system must continue operating while new capabilities are introduced.

The architecture should therefore allow new components to coexist with existing functionality during the transition.

---

## 8. Initial Business Goal

The first phase of the project is **not** to rebuild the entire fulfillment platform.

The immediate goal is to establish the foundation required for independently operating services.

Operations specifically wants engineering to answer:

> **How will the new platform know which service instances exist, which responsibilities they currently own, and whether those instances are still healthy?**

Engineering has been asked to produce an initial design and prototype addressing this problem.
