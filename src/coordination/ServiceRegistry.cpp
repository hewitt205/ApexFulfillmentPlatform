#include "ServiceRegistry.h"

namespace apex::coordination {

bool ServiceRegistry::registerInstance(const std::string& id, const std::string& role,
                                        Lease::Duration ttl) {
    auto it = instances_.find(id);
    if (it != instances_.end()) {
        if (it->second.role != role) {
            // Same id, different role: treat as a caller error rather than
            // silently reassigning ownership of an instance id.
            return false;
        }
        it->second.lease.renew();
        return true;
    }

    instances_.emplace(id, ServiceInstance{id, role, Lease(ttl)});
    return true;
}

bool ServiceRegistry::renew(const std::string& id) {
    auto it = instances_.find(id);
    if (it == instances_.end()) {
        return false;
    }
    it->second.lease.renew();
    return true;
}

std::size_t ServiceRegistry::reapExpired() {
    std::size_t removed = 0;
    for (auto it = instances_.begin(); it != instances_.end();) {
        if (it->second.lease.isExpired()) {
            it = instances_.erase(it);
            ++removed;
        } else {
            ++it;
        }
    }
    return removed;
}

std::vector<std::string> ServiceRegistry::aliveInstances(const std::string& role) const {
    std::vector<std::string> result;
    for (const auto& [id, instance] : instances_) {
        if (instance.role == role && !instance.lease.isExpired()) {
            result.push_back(id);
        }
    }
    return result;
}

bool ServiceRegistry::isRegistered(const std::string& id) const {
    auto it = instances_.find(id);
    return it != instances_.end() && !it->second.lease.isExpired();
}

} // namespace apex::coordination
