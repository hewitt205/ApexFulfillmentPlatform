#include "TestFramework.h"
#include "domain/InventoryItem.h"
#include "domain/Order.h"

using namespace apex::domain;
using namespace apex::test;

APEX_TEST(inventory_item_starts_with_full_availability) {
    InventoryItem item("SKU-1", "warehouse-1", 10);
    APEX_CHECK(item.onHand() == 10);
    APEX_CHECK(item.reserved() == 0);
    APEX_CHECK(item.available() == 10);
}

APEX_TEST(reserve_reduces_available_quantity) {
    InventoryItem item("SKU-1", "warehouse-1", 10);
    item.reserve(4);
    APEX_CHECK(item.reserved() == 4);
    APEX_CHECK(item.available() == 6);
}

APEX_TEST(reserve_more_than_available_throws) {
    InventoryItem item("SKU-1", "warehouse-1", 10);
    item.reserve(8);
    APEX_CHECK_THROWS(item.reserve(3));
}

APEX_TEST(release_reservation_restores_availability) {
    InventoryItem item("SKU-1", "warehouse-1", 10);
    item.reserve(6);
    item.releaseReservation(2);
    APEX_CHECK(item.reserved() == 4);
    APEX_CHECK(item.available() == 6);
}

APEX_TEST(release_more_than_reserved_throws) {
    InventoryItem item("SKU-1", "warehouse-1", 10);
    item.reserve(2);
    APEX_CHECK_THROWS(item.releaseReservation(5));
}

APEX_TEST(negative_on_hand_throws_on_construction) {
    APEX_CHECK_THROWS(InventoryItem("SKU-1", "warehouse-1", -1));
}

APEX_TEST(order_tracks_lines_and_status) {
    Order order("ORDER-1", "customer-1");
    order.addLine("SKU-1", 2);
    order.addLine("SKU-2", 1);

    APEX_CHECK(order.lines().size() == 2);
    APEX_CHECK(order.status() == OrderStatus::Placed);

    order.setStatus(OrderStatus::InventoryReserved);
    APEX_CHECK(order.status() == OrderStatus::InventoryReserved);
}
