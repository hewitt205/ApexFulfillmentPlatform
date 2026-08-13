#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Lease.h"

namespace apex::coordination {

// Metadata about a single registered service instance.
struct ServiceInstance {
    std::string id;       // unique instance id, e.g. "inventory-01"
    std::string role;     // logical service this instance belongs to, e.g. "inventory"
    Lease lease;
};

// ServiceRegistry tracks the set of currently-registered service instances
// and their leases. This is an in-process stand-in for what would, in a
// real multi-process system, be backed by a network-accessible coordination
// service (see docs/adr/ADR-001-coordination-approach.md).
class ServiceRegistry {
public:
    // Registers a new instance, or re-registers/renews an existing one with
    // the same id. Returns false if the id is already registered under a
    // *different* role (a caller error we want to surface, not silently allow).
    bool registerInstance(const std::string& id, const std::string& role,
                           Lease::Duration ttl);

    // Renews the lease for an already-registered instance. Returns false if
    // the id is not currently registered (e.g. it already expired and was
    // reaped).
    bool renew(const std::string& id);

    // Removes expired instances from the registry. Returns the number of
    // instances removed.
    std::size_t reapExpired();

    // Returns the ids of all currently-registered, non-expired instances
    // for a given role, without mutating the registry (does not reap).
    std::vector<std::string> aliveInstances(const std::string& role) const;

    bool isRegistered(const std::string& id) const;

    std::size_t size() const { return instances_.size(); }

private:
    std::unordered_map<std::string, ServiceInstance> instances_;
};

} // namespace apex::coordination
