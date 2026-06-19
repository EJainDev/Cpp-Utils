export module annotest:contracts;

import std;

export namespace annotest {
inline bool contract_violation_occurred = false;
inline std::atomic<void (*)(const std::contracts::contract_violation&)> custom_handler{nullptr};
}  // namespace annotest