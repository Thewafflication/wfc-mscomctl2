// The behaviour of a scroll bar, apart from any window: the range, the
// position and the steps. The FlatScrollBar control follows the intrinsic
// HScrollBar and VScrollBar, so the WFC evaluator uses this model for all
// three, and so do the COM objects of the library. Header only, with no
// dependency on the evaluator or on Windows. Internal to the WFC libraries.
//
// The behaviour follows the documented control (there is no reference probe of
// the Microsoft control yet, see COMPATIBILITY.md).

#ifndef WFC_MSCOMCTL2_SCROLLBAR_HPP
#define WFC_MSCOMCTL2_SCROLLBAR_HPP

#include <algorithm>

namespace wfc::mscomctl2 {

class ScrollBarModel final {
public:
    // The result of an operation that can change Value.
    struct Outcome {
        // False when the property value is invalid (error 380); nothing
        // changed.
        bool valid = true;
        // Value is not what it was.
        bool changed = false;
    };

    ScrollBarModel() = default;

    // A control that holds these properties already.
    ScrollBarModel(const long minimum, const long maximum, const long value,
                   const long small_change, const long large_change)
        : min_(minimum),
          max_(maximum),
          value_(value),
          small_(small_change),
          large_(large_change) {}

    [[nodiscard]] long min() const { return min_; }
    [[nodiscard]] long max() const { return max_; }
    [[nodiscard]] long value() const { return value_; }
    [[nodiscard]] long small_change() const { return small_; }
    [[nodiscard]] long large_change() const { return large_; }

    // The ends of the range; Min may be above Max.
    [[nodiscard]] long low() const { return std::min(min_, max_); }
    [[nodiscard]] long high() const { return std::max(min_, max_); }

    void set_min(const long minimum) { min_ = minimum; }
    void set_max(const long maximum) { max_ = maximum; }
    void set_small_change(const long amount) { small_ = amount; }
    void set_large_change(const long amount) { large_ = amount; }

    // Value outside the range is an invalid property value.
    [[nodiscard]] Outcome set_value(const long wanted) {
        if (wanted < low() || wanted > high()) {
            return {false, false};
        }
        const bool changed = wanted != value_;
        value_ = wanted;
        return {true, changed};
    }

    // The user moves the bar to `target` (the thumb dragged, or a step): the
    // position is kept inside the range.
    [[nodiscard]] Outcome move_to(const long target) {
        const long wanted = std::min(std::max(target, low()), high());
        const bool changed = wanted != value_;
        value_ = wanted;
        return {true, changed};
    }

    // A click on an arrow (SmallChange) or in the track (LargeChange). The
    // forward direction is towards Max.
    [[nodiscard]] Outcome step(const bool forward, const bool large) {
        const long amount = large ? large_ : small_;
        const long towards = (max_ >= min_) == forward ? amount : -amount;
        return move_to(value_ + towards);
    }

private:
    long min_ = 0;
    long max_ = 32767;
    long value_ = 0;
    long small_ = 1;
    long large_ = 1;
};

}  // namespace wfc::mscomctl2

#endif  // WFC_MSCOMCTL2_SCROLLBAR_HPP
