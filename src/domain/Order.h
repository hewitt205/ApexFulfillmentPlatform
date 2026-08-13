#pragma once

#include <string>
#include <vector>

namespace apex::domain {

enum class OrderStatus {
    Placed,
    InventoryReserved,
    Fulfilled,
    Cancelled,
};

struct OrderLine {
    std::string sku;
    int quantity;
};

// Order is a minimal domain object for M1. It intentionally does not know
// about persistence, APIs, or the coordination layer — those boundaries are
// introduced in M2/M3 once the requirements work (APEX-003, APEX-004) is
// done and there's an actual architecture to implement against.
class Order {
public:
    Order(std::string id, std::string customerId)
        : id_(std::move(id)), customerId_(std::move(customerId)) {}

    void addLine(const std::string& sku, int quantity) {
        lines_.push_back(OrderLine{sku, quantity});
    }

    const std::string& id() const { return id_; }
    const std::string& customerId() const { return customerId_; }
    const std::vector<OrderLine>& lines() const { return lines_; }
    OrderStatus status() const { return status_; }

    void setStatus(OrderStatus status) { status_ = status; }

private:
    std::string id_;
    std::string customerId_;
    std::vector<OrderLine> lines_;
    OrderStatus status_ = OrderStatus::Placed;
};

} // namespace apex::domain
