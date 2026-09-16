---
description: "Start an orchestrated plan-execute-review workflow for an inverted-pendulum-bot development task. Routes through planning, implementation, and code review stages with handoff buttons between each step."
agent: "orchestrator"
argument-hint: "Describe the feature, algorithm, bug fix, or change you want to implement"
model: "Claude Sonnet 4.6"
---

Analyze the following task for the **inverted-pendulum-bot** project — firmware for a
self-balancing inverted pendulum robot, with strict real-time and memory
constraints on a resource-constrained microcontroller, and a host build for
off-target testing.
Gather relevant context from the codebase — identify affected layers
(`core/<component>/interfaces/`, `core/<component>/implementations/`, `targets/`,
`tools/`), hardware target (the ST board or host), real-time timing implications,
and documentation requirements. Then provide a brief scope summary and use the
handoff buttons to route to the appropriate specialist:

- **Plan Implementation**: for complex tasks needing detailed upfront design (new
  components, new algorithms, architectural changes)
- **Execute Directly**: for straightforward changes with a clear path (bug fixes,
  small changes, adding tests)
- **Review Code**: for reviewing existing or recently changed code against standards

Task to orchestrate:
