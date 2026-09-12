#include "quantforge/order/order_lifecycle.hpp"

namespace quantforge::order {

OrderStatus OrderLifecycle::status() const noexcept {
    return status_;
}

bool OrderLifecycle::is_terminal() const noexcept {
    return
        status_ == OrderStatus::Filled ||
        status_ == OrderStatus::Rejected ||
        status_ == OrderStatus::Cancelled;
}

void OrderLifecycle::submit() {
    transition_to(OrderStatus::Submitted);
}

void OrderLifecycle::accept() {
    transition_to(OrderStatus::Accepted);
}

void OrderLifecycle::partially_fill() {
    transition_to(OrderStatus::PartiallyFilled);
}

void OrderLifecycle::fill() {
    transition_to(OrderStatus::Filled);
}

void OrderLifecycle::reject() {
    transition_to(OrderStatus::Rejected);
}

void OrderLifecycle::cancel() {
    transition_to(OrderStatus::Cancelled);
}

void OrderLifecycle::transition_to(OrderStatus next_status) {
    bool valid = false;

    switch (status_) {
        case OrderStatus::Created:
            valid =
                next_status == OrderStatus::Submitted ||
                next_status == OrderStatus::Rejected;

            break;

        case OrderStatus::Submitted:
            valid =
                next_status == OrderStatus::Accepted ||
                next_status == OrderStatus::Rejected ||
                next_status == OrderStatus::Cancelled;

            break;

        case OrderStatus::Accepted:
            valid =
                next_status == OrderStatus::PartiallyFilled ||
                next_status == OrderStatus::Filled ||
                next_status == OrderStatus::Cancelled;

            break;

        case OrderStatus::PartiallyFilled:
            valid =
                next_status == OrderStatus::PartiallyFilled ||
                next_status == OrderStatus::Filled ||
                next_status == OrderStatus::Cancelled;

            break;

        case OrderStatus::Filled:
        case OrderStatus::Rejected:
        case OrderStatus::Cancelled:
            valid = false;
            break;
    }

    if (!valid) {
        throw std::logic_error(
            "Invalid order lifecycle transition."
        );
    }

    status_ = next_status;
}

} // namespace quantforge::order
