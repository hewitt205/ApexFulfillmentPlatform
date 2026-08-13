# Senior-Engineer Workflow

This document is the reference version of the nine-step process referenced from the README. Every substantial piece of work in this repository should be traceable through these steps, even informally.

1. **Read the fictional company's requirements.**
   Start from `docs/COMPANY.md` and any ticket describing the request. Don't start from the solution.

2. **Identify the business and technical problems.**
   Separate "what the business is complaining about" from "what is actually broken underneath it." These are often different things.

3. **Determine what the system needs.**
   Translate problems into capabilities. Avoid jumping straight to a specific technology.

4. **Draw the architecture.**
   Even a rough diagram forces explicit decisions about boundaries and responsibilities.

5. **Identify the components.**
   Name the services/modules involved and what each one owns.

6. **Decide where an API belongs.**
   Which component is authoritative for a given piece of data or decision? What does its contract look like?

7. **Decide what the UI actually needs to observe/control.**
   Even before a UI exists, this constrains what the backend needs to expose. (Currently relevant for planning M6, the React operations console — no UI work should start before this step has real answers.)

8. **Record those decisions as architecture decisions.**
   An ADR under `docs/adr/` — short, dated, with context, decision, and consequences.

9. **Then build the pieces.**
   Implementation, tests, and — once something can fail — the failure-engineering loop (introduce failure, diagnose, fix, document) described in the README's Engineering Philosophy section.

## Applying this to a new piece of work

When picking up a ticket from `docs/TICKETS.md`:

- If it's a `requirements` or `architecture` ticket, steps 1–8 are the actual deliverable — there may be no code yet.
- If it's a `feature` ticket, steps 1–3 should already be settled by a prior requirements/architecture ticket it depends on; the feature ticket itself is mostly steps 5–9.
- If it's a `bug` or `incident` ticket, treat steps 1–3 as "what did we actually think this was supposed to do, and where did that assumption break," then follow the failure-engineering loop rather than jumping straight to a patch.

The point of writing this down explicitly is to make the *process* visible in the repository, not just the resulting code.
