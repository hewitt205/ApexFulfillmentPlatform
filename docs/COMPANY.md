# Company Brief — Apex Industrial Supply

## Overview

Apex Industrial Supply sells industrial parts (fasteners, tooling, safety equipment, replacement components) to businesses through three channels:

- a **desktop application** used by internal order desk staff;
- a **web portal** used by direct customers;
- a **partner API** used by distributors who resell Apex inventory.

All three channels currently write to the same monolithic backend and shared database.

## Business Background

Apex has grown from a regional supplier into a multi-warehouse operation over the last several years. The original system was built for a much smaller operation and has not scaled cleanly with that growth.

Order desk staff and customers have both raised recurring complaints, and the engineering team has recurring operational pain. The problems fall into two categories.

### Business-visible problems

- Order confirmation is noticeably slow during peak ordering windows (typically Monday mornings and end-of-quarter).
- Customers occasionally receive confirmation for orders that cannot actually be fulfilled because the inventory was already committed to another order.
- A failure in one part of the system (e.g., the partner API) has, on more than one occasion, taken down order placement for the web portal as well.
- Deploying a fix for one part of the system requires redeploying and re-verifying the entire application, which slows down releases and increases the blast radius of any single change.

### Engineering-visible problems

- There is no clear ownership boundary between "order," "inventory," and "shipping" concerns in the codebase — they are intertwined.
- Diagnosing an incident typically requires reading logs across the whole monolith rather than a single service, because it isn't clear which subsystem actually caused a given symptom.
- There is no formal mechanism for services (once split out) to discover each other, agree on leadership for coordinated work, or detect when a peer has failed.

## Business Goal

Move toward a set of **independently deployable services** — without a "big bang" rewrite, and without discarding the business rules that already work correctly (pricing, tax rules, customer account rules, fulfillment eligibility rules, etc.).

## Constraints (as understood so far)

- Existing business rules must be preserved unless a rule is explicitly identified as a bug.
- The order desk staff's desktop application cannot go down during business hours (9am–6pm across three time zones) without an approved maintenance window.
- Partner API contracts with distributors are governed by external agreements and cannot change without advance notice.
- There is no dedicated platform/infrastructure team yet — the engineering team is also responsible for whatever operational tooling it needs.

## Open Questions

These are intentionally left open as of M0. They should be revisited once real requirements work begins (see `docs/TICKETS.md`, ticket APEX-003):

- What does "independently deployable" mean in practice for this team — separate processes? Separate repos? Separate deploy pipelines?
- What level of consistency is actually required between inventory and order placement (strict vs. eventual)?
- Which failures are acceptable to surface to the customer vs. which must be invisible?
- Who owns the coordination layer once it exists — is it a shared platform concern or does each service reimplement it?

## Status

This is a fictional company used to frame realistic engineering decisions. Requirements below are treated as if they came from a real product/engineering organization, and are deliberately somewhat incomplete and ambiguous — analyzing that ambiguity is part of the exercise (see `docs/ENGINEERING-WORKFLOW.md`).
