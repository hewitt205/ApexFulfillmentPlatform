#pragma once

#include <optional>
#include <string>

#include "ServiceRegistry.h"

namespace apex::coordination {

// LeaderElection picks a leader deterministically from the currently-alive
// instances of a given role in the registry.
//
// Policy (M1, intentionally simple): the lexicographically smallest alive
// instance id wins. This is deterministic and requires no additional
// coordination state, which makes it easy to reason about and test — but it
// is not fair (the same instance tends to keep winning) and is not resilient
// to network partitions between coordination replicas. Both of those are
// explicit trade-offs recorded in docs/adr/ADR-001-coordination-approach.md
// and are candidates for the M5 failure-engineering exercises.
class LeaderElection {
public:
    explicit LeaderElection(const ServiceRegistry& registry) : registry_(registry) {}

    // Returns the current leader for a role, or std::nullopt if there are no
    // alive instances of that role.
    std::optional<std::string> currentLeader(const std::string& role) const;

private:
    const ServiceRegistry& registry_;
};

} // namespace apex::coordination
