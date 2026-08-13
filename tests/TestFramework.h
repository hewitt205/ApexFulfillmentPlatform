#pragma once

// Deliberately minimal, dependency-free test scaffolding for M1 — see
// docs/adr/ADR-001-coordination-approach.md for the "no third-party runtime
// dependencies" constraint this respects. A real framework (Catch2/GTest)
// can be introduced later via CMake FetchContent once that trade-off is
// worth revisiting; that decision should get its own ADR when it happens.

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace apex::test {

struct TestCase {
    std::string name;
    void (*fn)();
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> instance;
    return instance;
}

struct Registrar {
    Registrar(const std::string& name, void (*fn)()) {
        registry().push_back({name, fn});
    }
};

inline int runAll() {
    int failures = 0;
    for (const auto& t : registry()) {
        try {
            t.fn();
            std::cout << "[PASS] " << t.name << "\n";
        } catch (const std::exception& e) {
            std::cout << "[FAIL] " << t.name << " -- " << e.what() << "\n";
            ++failures;
        } catch (...) {
            std::cout << "[FAIL] " << t.name << " -- unknown exception\n";
            ++failures;
        }
    }
    std::cout << "----------------------------------------------------\n";
    std::cout << (registry().size() - failures) << "/" << registry().size()
              << " tests passed\n";
    return failures == 0 ? 0 : 1;
}

} // namespace apex::test

#define APEX_TEST(name)                                                       \
    void name();                                                              \
    static apex::test::Registrar registrar_##name(#name, name);               \
    void name()

#define APEX_CHECK(cond)                                                      \
    do {                                                                      \
        if (!(cond)) {                                                        \
            std::ostringstream oss;                                           \
            oss << "check failed: " #cond " (" << __FILE__ << ":" << __LINE__ \
                << ")";                                                       \
            throw std::runtime_error(oss.str());                              \
        }                                                                     \
    } while (0)

#define APEX_CHECK_THROWS(expr)                                               \
    do {                                                                      \
        bool threw = false;                                                   \
        try {                                                                 \
            (expr);                                                          \
        } catch (...) {                                                       \
            threw = true;                                                     \
        }                                                                     \
        if (!threw) {                                                        \
            throw std::runtime_error("expected exception from: " #expr);      \
        }                                                                     \
    } while (0)
