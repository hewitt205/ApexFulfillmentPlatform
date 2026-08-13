#pragma once

#include <chrono>

namespace apex::coordination {

// A Lease represents a time-bounded claim held by a registered service
// instance. If the lease is not renewed before it expires, the instance
// is considered dead by the registry.
class Lease {
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::milliseconds;

    explicit Lease(Duration ttl)
        : ttl_(ttl), expiresAt_(Clock::now() + ttl) {}

    // Renew the lease for another full TTL window, starting now.
    void renew() {
        expiresAt_ = Clock::now() + ttl_;
    }

    bool isExpired() const {
        return Clock::now() >= expiresAt_;
    }

    Duration ttl() const { return ttl_; }

    Clock::time_point expiresAt() const { return expiresAt_; }

private:
    Duration ttl_;
    Clock::time_point expiresAt_;
};

} // namespace apex::coordination
