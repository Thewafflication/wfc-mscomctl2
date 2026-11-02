// The behaviour of the DTPicker control, apart from any window: the date and
// time it holds (or Null while CheckBox is cleared), the range, and the rules
// for its parts. Header only, with no dependency on the WFC evaluator or on
// Windows; the evaluator, the COM objects of the library and the tests all use
// it. Internal to the WFC libraries.
//
// The value is a date serial with the time of day as its fraction. The
// behaviour follows the documented control (there is no reference probe of the
// Microsoft control yet, see COMPATIBILITY.md).

#ifndef WFC_MSCOMCTL2_DTPICKER_HPP
#define WFC_MSCOMCTL2_DTPICKER_HPP

#include <wfc/mscomctl2/calendar.hpp>

#include <algorithm>
#include <cmath>

namespace wfc::mscomctl2 {

class DateTimePickerModel final {
public:
    enum class Part { Year, Month, Day, Hour, Minute, Second };

    struct Outcome {
        // False when the value is invalid (error 380) or the operation does not
        // apply; nothing changed.
        bool valid = true;
        // Value is not what it was (the program raises Change).
        bool changed = false;
    };

    // What a control that holds these already keeps (the caller keeps the range
    // in order and the value inside it).
    struct State {
        double value = 0;
        bool null = false;
        // The date a ticked box shows when no value was kept before.
        double initial = 0;
        long min = calendar::first_date();
        long max = calendar::last_date();
        bool checkbox = false;
        bool has_last = false;
        double last = 0;
    };

    // A new control shows `initial` (now unless the form says otherwise).
    explicit DateTimePickerModel(const double initial) {
        state_.value = initial;
        state_.initial = initial;
    }

    explicit DateTimePickerModel(const State& state) : state_(state) {}

    [[nodiscard]] const State& state() const { return state_; }
    [[nodiscard]] double value() const { return state_.value; }
    [[nodiscard]] bool is_null() const { return state_.null; }
    [[nodiscard]] long min() const { return state_.min; }
    [[nodiscard]] long max() const { return state_.max; }
    [[nodiscard]] bool checkbox() const { return state_.checkbox; }

    void set_checkbox(const bool checkbox) { state_.checkbox = checkbox; }

    // The parts of Value; meaningless while it is Null.
    [[nodiscard]] calendar::DateTime parts() const {
        return calendar::datetime_parts(state_.value);
    }

    [[nodiscard]] bool in_range(const double value) const {
        const double day = std::floor(value);
        return day >= static_cast<double>(state_.min) &&
               day <= static_cast<double>(state_.max);
    }

    // Value, by code or by the user: inside MinDate and MaxDate.
    [[nodiscard]] Outcome set_value(const double value) {
        if (!in_range(value)) {
            return {false, false};
        }
        const bool changed = state_.null || value != state_.value;
        state_.value = value;
        state_.null = false;
        return {true, changed};
    }

    // Value = Null: only while CheckBox is True.
    [[nodiscard]] Outcome set_null() {
        if (!state_.checkbox) {
            return {false, false};
        }
        const bool changed = !state_.null;
        state_.null = true;
        return {true, changed};
    }

    // MinDate and MaxDate: the range stays ordered and Value (when it is not
    // Null) is brought inside it, keeping its time of day.
    [[nodiscard]] Outcome set_min(const long date) {
        if (date > state_.max) {
            return {false, false};
        }
        state_.min = date;
        return clamp();
    }

    [[nodiscard]] Outcome set_max(const long date) {
        if (date < state_.min) {
            return {false, false};
        }
        state_.max = date;
        return clamp();
    }

    // Day, Month, Year, Hour, Minute or Second of Value. A day beyond the end
    // of the month is invalid for Day and brought back for Month and Year.
    [[nodiscard]] Outcome set_part(const Part part, const long number) {
        if (state_.null) {
            return {false, false};
        }
        const calendar::DateTime of = parts();
        long year = of.year;
        long month = of.month;
        long day = of.day;
        long hour = of.hour;
        long minute = of.minute;
        long second = of.second;
        switch (part) {
            case Part::Year:
                year = number;
                break;
            case Part::Month:
                month = number;
                break;
            case Part::Day:
                day = number;
                break;
            case Part::Hour:
                hour = number;
                break;
            case Part::Minute:
                minute = number;
                break;
            case Part::Second:
                second = number;
                break;
        }
        if (year < calendar::kFirstYear || year > calendar::kLastYear ||
            month < 1 || month > 12 || day < 1 || hour < 0 || hour > 23 ||
            minute < 0 || minute > 59 || second < 0 || second > 59 ||
            (part == Part::Day && day > calendar::days_in_month(year, month))) {
            return {false, false};
        }
        day = std::min(day, calendar::days_in_month(year, month));
        return set_value(
            static_cast<double>(calendar::serial(year, month, day)) +
            calendar::time_fraction(hour, minute, second));
    }

    // The user ticks or clears the box. Clearing keeps the value for the next
    // tick; ticking shows that value, or `initial` the first time. Nothing
    // applies without CheckBox or when the box already is as asked.
    [[nodiscard]] Outcome check(const bool on) {
        if (!state_.checkbox || on == !state_.null) {
            return {false, false};
        }
        if (on) {
            state_.value = state_.has_last ? state_.last : state_.initial;
            state_.null = false;
        } else {
            state_.last = state_.value;
            state_.has_last = true;
            state_.null = true;
        }
        return {true, true};
    }

private:
    [[nodiscard]] Outcome clamp() {
        if (state_.null) {
            return {};
        }
        const double day = std::floor(state_.value);
        const double fraction = state_.value - day;
        if (day < static_cast<double>(state_.min)) {
            state_.value = static_cast<double>(state_.min) + fraction;
        } else if (day > static_cast<double>(state_.max)) {
            state_.value = static_cast<double>(state_.max) + fraction;
        }
        return {};
    }

    State state_;
};

}  // namespace wfc::mscomctl2

#endif  // WFC_MSCOMCTL2_DTPICKER_HPP
