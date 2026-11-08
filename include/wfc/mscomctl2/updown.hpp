// The behaviour of the UpDown control, apart from any window: the range, the
// steps of the arrows and the rules for the properties. Header only, with no
// dependency on the WFC evaluator or on Windows; the evaluator, the COM objects
// of the library and the tests all use it. Internal to the WFC libraries.
//
// The behaviour follows the documented control (there is no reference probe of
// the Microsoft control yet, see COMPATIBILITY.md).

#ifndef WFC_MSCOMCTL2_UPDOWN_HPP
#define WFC_MSCOMCTL2_UPDOWN_HPP

#include <algorithm>

namespace wfc::mscomctl2 {

class UpDownModel final {
public:
    // The result of an operation that can change Value.
    struct Outcome {
        // False when the property value is invalid (error 380); nothing
        // changed.
        bool valid = true;
        // Value is not what it was.
        bool changed = false;
        // For an arrow or a position: the arrow that counts as clicked.
        bool up = false;
    };

    UpDownModel() = default;

    // A control that holds these properties already (the caller keeps them
    // ordered and Value inside the range).
    UpDownModel(const long minimum, const long maximum, const long value,
                const long increment, const bool wrap)
        : min_(minimum),
          max_(maximum),
          value_(value),
          increment_(increment),
          wrap_(wrap) {}

    [[nodiscard]] long min() const { return min_; }
    [[nodiscard]] long max() const { return max_; }
    [[nodiscard]] long value() const { return value_; }
    [[nodiscard]] long increment() const { return increment_; }
    [[nodiscard]] bool wrap() const { return wrap_; }

    void set_wrap(const bool wrap) { wrap_ = wrap; }

    // Increment below 1 acts as 1 when the arrows are clicked.
    void set_increment(const long increment) { increment_ = increment; }

    // Min and Max take any number (measured under VB6: Max below Min is
    // accepted); Value is brought inside the range.
    [[nodiscard]] Outcome set_min(const long minimum) {
        min_ = minimum;
        return clamp();
    }

    [[nodiscard]] Outcome set_max(const long maximum) {
        max_ = maximum;
        return clamp();
    }

    // Value outside Min and Max is an invalid property value.
    [[nodiscard]] Outcome set_value(const long wanted) {
        if (wanted < min_ || wanted > max_) {
            return {false, false, false};
        }
        const bool changed = wanted != value_;
        value_ = wanted;
        return {true, changed, false};
    }

    // The user clicks an arrow: Value steps by Increment, and goes round from
    // one end to the other with Wrap or stops at the end without it.
    [[nodiscard]] Outcome click(const bool up) {
        const long step = std::max(1L, increment_);
        long wanted = up ? value_ + step : value_ - step;
        if (wanted > max_) {
            wanted = wrap_ ? min_ : max_;
        } else if (wanted < min_) {
            wanted = wrap_ ? max_ : min_;
        }
        const bool changed = wanted != value_;
        value_ = wanted;
        return {true, changed, up};
    }

    // The window reports the new position of its arrows; it is kept inside the
    // range, and the arrow is the one that moved it up or down.
    [[nodiscard]] Outcome position(const long reported) {
        const long wanted = std::min(std::max(reported, min_), max_);
        const bool up = wanted >= value_;
        const bool changed = wanted != value_;
        value_ = wanted;
        return {true, changed, up};
    }

private:
    [[nodiscard]] Outcome clamp() {
        const long before = value_;
        value_ = std::min(std::max(value_, min_), std::max(min_, max_));
        return {true, value_ != before, false};
    }

    long min_ = 0;
    long max_ = 10;
    long value_ = 0;
    long increment_ = 1;
    bool wrap_ = false;
};

}  // namespace wfc::mscomctl2

#endif  // WFC_MSCOMCTL2_UPDOWN_HPP
