#include <iostream>
#include <thread>

#include "coordination/LeaderElection.h"
#include "coordination/ServiceRegistry.h"
#include "domain/InventoryItem.h"
#include "domain/Order.h"

using namespace apex;

// This is a deliberately small demonstration executable, not a service.
// It exists to exercise the coordination model end-to-end (M1) before any
// real network boundary exists (M2). See README.md "Current Status".
int main() {
    std::cout << "Apex Fulfillment Platform -- M1 coordination demo\n";
    std::cout << "----------------------------------------------------\n";

    coordination::ServiceRegistry registry;

    registry.registerInstance("inventory-01", "inventory", std::chrono::seconds(5));
    registry.registerInstance("inventory-02", "inventory", std::chrono::seconds(5));
    registry.registerInstance("order-01", "order", std::chrono::seconds(5));

    std::cout << "Registered instances: " << registry.size() << "\n";

    coordination::LeaderElection election(registry);
    auto leader = election.currentLeader("inventory");
    std::cout << "Current inventory leader: "
              << (leader ? *leader : std::string("<none>")) << "\n";

    std::cout << "\nSimulating a short lease TTL to demonstrate expiration...\n";
    coordination::ServiceRegistry shortLivedRegistry;
    shortLivedRegistry.registerInstance("inventory-01", "inventory", std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto reaped = shortLivedRegistry.reapExpired();
    std::cout << "Reaped " << reaped << " expired instance(s) after TTL elapsed.\n";

    std::cout << "\nDomain object sanity check:\n";
    domain::InventoryItem widget("WIDGET-100", "warehouse-3", 10);
    widget.reserve(4);
    std::cout << "WIDGET-100 on-hand=" << widget.onHand()
              << " reserved=" << widget.reserved()
              << " available=" << widget.available() << "\n";

    domain::Order order("ORDER-1001", "customer-42");
    order.addLine("WIDGET-100", 4);
    order.setStatus(domain::OrderStatus::InventoryReserved);
    std::cout << "Order " << order.id() << " has " << order.lines().size()
              << " line(s), status=InventoryReserved\n";

    std::cout << "\nDone. See docs/TICKETS.md for planned next steps (APEX-003 onward).\n";
    return 0;
}
