#pragma once

#include <stdexcept>

namespace quantforge::order {

enum class OrderStatus {
    Created,
    Submitted,
    Accepted,
    PartiallyFilled,
    Filled,
    Rejected,
    Cancelled
};

class OrderLifecycle {
public:
    OrderLifecycle() noexcept = default;

    [[nodiscard]] OrderStatus status() const noexcept;

    [[nodiscard]] bool is_terminal() const noexcept;

    void submit();

    void accept();

    void partially_fill();

    void fill();

    void reject();

    void cancel();

private:
    void transition_to(OrderStatus next_status);

    OrderStatus status_ = OrderStatus::Created;
};

} // namespace quantforge::order
