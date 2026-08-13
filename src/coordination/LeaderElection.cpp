#include "LeaderElection.h"

#include <algorithm>

namespace apex::coordination {

std::optional<std::string> LeaderElection::currentLeader(const std::string& role) const {
    auto alive = registry_.aliveInstances(role);
    if (alive.empty()) {
        return std::nullopt;
    }
    return *std::min_element(alive.begin(), alive.end());
}

} // namespace apex::coordination
