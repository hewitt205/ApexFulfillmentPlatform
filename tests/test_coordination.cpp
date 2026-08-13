#include <thread>

#include "TestFramework.h"
#include "coordination/LeaderElection.h"
#include "coordination/ServiceRegistry.h"

using namespace apex::coordination;
using namespace apex::test;

APEX_TEST(register_new_instance_succeeds) {
    ServiceRegistry registry;
    APEX_CHECK(registry.registerInstance("inventory-01", "inventory", std::chrono::seconds(5)));
    APEX_CHECK(registry.isRegistered("inventory-01"));
    APEX_CHECK(registry.size() == 1);
}

APEX_TEST(registering_same_id_different_role_fails) {
    ServiceRegistry registry;
    APEX_CHECK(registry.registerInstance("svc-01", "inventory", std::chrono::seconds(5)));
    APEX_CHECK(!registry.registerInstance("svc-01", "order", std::chrono::seconds(5)));
}

APEX_TEST(re_registering_same_id_same_role_renews) {
    ServiceRegistry registry;
    registry.registerInstance("inventory-01", "inventory", std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    APEX_CHECK(registry.registerInstance("inventory-01", "inventory", std::chrono::milliseconds(50)));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    // Total elapsed since first registration is 60ms, but the renewal at
    // 30ms should have pushed the deadline out, so it should still be alive.
    APEX_CHECK(registry.isRegistered("inventory-01"));
}

APEX_TEST(expired_lease_is_not_registered) {
    ServiceRegistry registry;
    registry.registerInstance("inventory-01", "inventory", std::chrono::milliseconds(10));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    APEX_CHECK(!registry.isRegistered("inventory-01"));
}

APEX_TEST(reap_expired_removes_only_dead_instances) {
    ServiceRegistry registry;
    registry.registerInstance("short-lived", "inventory", std::chrono::milliseconds(10));
    registry.registerInstance("long-lived", "inventory", std::chrono::seconds(5));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    auto removed = registry.reapExpired();
    APEX_CHECK(removed == 1);
    APEX_CHECK(registry.size() == 1);
    APEX_CHECK(registry.isRegistered("long-lived"));
}

APEX_TEST(renew_unknown_instance_fails) {
    ServiceRegistry registry;
    APEX_CHECK(!registry.renew("does-not-exist"));
}

APEX_TEST(alive_instances_filters_by_role) {
    ServiceRegistry registry;
    registry.registerInstance("inventory-01", "inventory", std::chrono::seconds(5));
    registry.registerInstance("order-01", "order", std::chrono::seconds(5));

    auto inventoryInstances = registry.aliveInstances("inventory");
    APEX_CHECK(inventoryInstances.size() == 1);
    APEX_CHECK(inventoryInstances[0] == "inventory-01");
}

APEX_TEST(leader_election_picks_lexicographically_smallest_alive_id) {
    ServiceRegistry registry;
    registry.registerInstance("inventory-02", "inventory", std::chrono::seconds(5));
    registry.registerInstance("inventory-01", "inventory", std::chrono::seconds(5));

    LeaderElection election(registry);
    auto leader = election.currentLeader("inventory");
    APEX_CHECK(leader.has_value());
    APEX_CHECK(*leader == "inventory-01");
}

APEX_TEST(leader_election_returns_nullopt_when_no_alive_instances) {
    ServiceRegistry registry;
    LeaderElection election(registry);
    APEX_CHECK(!election.currentLeader("inventory").has_value());
}

APEX_TEST(leader_election_excludes_expired_instances) {
    ServiceRegistry registry;
    registry.registerInstance("inventory-01", "inventory", std::chrono::milliseconds(10));
    registry.registerInstance("inventory-02", "inventory", std::chrono::seconds(5));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    LeaderElection election(registry);
    auto leader = election.currentLeader("inventory");
    APEX_CHECK(leader.has_value());
    APEX_CHECK(*leader == "inventory-02");
}

int main() {
    return apex::test::runAll();
}
