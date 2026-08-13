#pragma once

#include <stdexcept>
#include <string>

namespace apex::domain {

// InventoryItem is a minimal domain object representing on-hand and
// reserved quantity for a single SKU at a single warehouse.
//
// This is intentionally free of any persistence, transport, or concurrency
// concerns for M1 — it exists to establish the shape of the domain model
// that the coordination layer will eventually sit in front of (M3).
class InventoryItem {
public:
    InventoryItem(std::string sku, std::string warehouseId, int onHand)
        : sku_(std::move(sku)), warehouseId_(std::move(warehouseId)) {
        if (onHand < 0) {
            throw std::invalid_argument("onHand cannot be negative");
        }
        onHand_ = onHand;
    }

    const std::string& sku() const { return sku_; }
    const std::string& warehouseId() const { return warehouseId_; }
    int onHand() const { return onHand_; }
    int reserved() const { return reserved_; }
    int available() const { return onHand_ - reserved_; }

    // Reserves `quantity` units against this item. Throws if there isn't
    // enough available quantity — this is the invariant that a real
    // reservation API (M3) will need to enforce under concurrency, which is
    // exactly the race condition described in docs/COMPANY.md.
    void reserve(int quantity) {
        if (quantity <= 0) {
            throw std::invalid_argument("reserve quantity must be positive");
        }
        if (quantity > available()) {
            throw std::runtime_error("insufficient available inventory for " + sku_);
        }
        reserved_ += quantity;
    }

    void releaseReservation(int quantity) {
        if (quantity <= 0) {
            throw std::invalid_argument("release quantity must be positive");
        }
        if (quantity > reserved_) {
            throw std::runtime_error("cannot release more than is reserved for " + sku_);
        }
        reserved_ -= quantity;
    }

private:
    std::string sku_;
    std::string warehouseId_;
    int onHand_ = 0;
    int reserved_ = 0;
};

} // namespace apex::domain
