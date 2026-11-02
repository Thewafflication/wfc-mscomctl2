// The behaviour of the MonthView control, apart from any window: the dates, the
// selection, the range and the days the calendar shows. Header only, with no
// dependency on the WFC evaluator or on Windows; the evaluator, the COM objects
// of the library and the tests all use it. Internal to the WFC libraries.
//
// Dates are whole date serials (days since 1899-12-30, as Visual Basic counts
// them). The behaviour follows the documented control (there is no reference
// probe of the Microsoft control yet, see COMPATIBILITY.md).

#ifndef WFC_MSCOMCTL2_MONTHVIEW_HPP
#define WFC_MSCOMCTL2_MONTHVIEW_HPP

#include <wfc/mscomctl2/calendar.hpp>

#include <algorithm>
#include <set>

namespace wfc::mscomctl2 {

// ----- The days shown in bold ------------------------------------------------

class DayBold final {
public:
    [[nodiscard]] bool is_bold(const long date) const {
        return days_.contains(date);
    }

    void set(const long date, const bool bold) {
        if (bold) {
            days_.insert(date);
        } else {
            days_.erase(date);
        }
    }

    [[nodiscard]] const std::set<long>& days() const { return days_; }

    bool operator==(const DayBold& other) const = default;

private:
    std::set<long> days_;
};

// ----- The model
// ---------------------------------------------------------------

class MonthViewModel final {
public:
    enum class Part { Day, Month, Year };

    struct Outcome {
        // False when the value is invalid (error 380); nothing changed.
        bool valid = true;
    };

    // What the user's selection does (see select()).
    struct Selection {
        bool accepted = false;  // inside the range and not too long
        bool changed = false;   // differs from the selection now
        long start = 0;
        long end = 0;
    };

    // A new control selects `value` (today unless the form says otherwise).
    explicit MonthViewModel(const long value)
        : value_(value), start_(value), end_(value) {}

    // A control that holds these already (the caller keeps them in order).
    MonthViewModel(const long value, const long start, const long end,
                   const long minimum, const long maximum)
        : value_(value),
          start_(start),
          end_(end),
          min_(minimum),
          max_(maximum) {}

    [[nodiscard]] long value() const { return value_; }
    [[nodiscard]] long sel_start() const { return start_; }
    [[nodiscard]] long sel_end() const { return end_; }
    [[nodiscard]] long min() const { return min_; }
    [[nodiscard]] long max() const { return max_; }
    [[nodiscard]] bool multi_select() const { return multi_; }
    [[nodiscard]] long max_sel_count() const { return most_; }
    [[nodiscard]] long first_day() const { return first_day_; }

    void set_multi_select(const bool multi) { multi_ = multi; }
    void set_max_sel_count(const long most) { most_ = std::max(1L, most); }

    // The first day of the week, 1 (Sunday) to 7; anything else is Sunday. A
    // form file stores the value with flag bits above the day.
    void set_first_day(const long day) {
        const long low = day & 0xFFFF;
        first_day_ = low >= 1 && low <= 7 ? low : 1;
    }

    [[nodiscard]] calendar::Parts parts() const {
        return calendar::parts(value_);
    }

    [[nodiscard]] long week() const {
        return calendar::week_of_year(value_, first_day_);
    }

    // The first of the 42 days the calendar shows for the month of Value.
    [[nodiscard]] long first_visible() const {
        const calendar::Parts of = parts();
        const long first_of_month = calendar::serial(of.year, of.month, 1);
        const long weekday = calendar::parts(first_of_month).weekday;
        return first_of_month - (weekday - first_day_ + 7) % 7;
    }

    // VisibleDays(index), index 1 to 42; false outside.
    [[nodiscard]] bool visible_day(const long index, long& date) const {
        if (index < 1 || index > 42) {
            return false;
        }
        date = first_visible() + index - 1;
        return true;
    }

    // Value, or SelStart/SelEnd without MultiSelect: `date` becomes the only
    // selected date.
    [[nodiscard]] Outcome set_value(const long date) {
        if (!in_range(date)) {
            return {false};
        }
        select_one(date);
        return {};
    }

    [[nodiscard]] Outcome set_sel_start(const long date) {
        return set_selection(date, true);
    }

    [[nodiscard]] Outcome set_sel_end(const long date) {
        return set_selection(date, false);
    }

    // MinDate and MaxDate: the range stays ordered and Value is brought inside.
    [[nodiscard]] Outcome set_min(const long date) {
        if (date > max_) {
            return {false};
        }
        min_ = date;
        if (value_ < min_) {
            select_one(min_);
        }
        return {};
    }

    [[nodiscard]] Outcome set_max(const long date) {
        if (date < min_) {
            return {false};
        }
        max_ = date;
        if (value_ > max_) {
            select_one(max_);
        }
        return {};
    }

    // Day, Month or Year of Value: a day beyond the end of the month is invalid
    // for Day and brought back to the end of the month for Month and Year.
    [[nodiscard]] Outcome set_part(const Part part, const long number) {
        const calendar::Parts of = parts();
        long year = of.year;
        long month = of.month;
        long day = of.day;
        if (part == Part::Year) {
            year = number;
        } else if (part == Part::Month) {
            month = number;
        } else {
            day = number;
        }
        if (year < calendar::kFirstYear || year > calendar::kLastYear ||
            month < 1 || month > 12 || day < 1 ||
            (part == Part::Day && day > calendar::days_in_month(year, month))) {
            return {false};
        }
        day = std::min(day, calendar::days_in_month(year, month));
        const long date = calendar::serial(year, month, day);
        if (!in_range(date)) {
            return {false};
        }
        select_one(date);
        return {};
    }

    // The user selects `start` to `end` (one date without MultiSelect). Nothing
    // is changed here: the caller raises SelChange, which may cancel, and then
    // calls apply().
    [[nodiscard]] Selection select(long start, long end) const {
        Selection result;
        if (!in_range(start) || !in_range(end)) {
            return result;
        }
        if (!multi_) {
            end = start;
        }
        if (end < start) {
            std::swap(start, end);
        }
        if (end - start + 1 > most_) {
            return result;
        }
        result.accepted = true;
        result.changed = start != start_ || end != end_;
        result.start = start;
        result.end = end;
        return result;
    }

    void apply(const Selection& selection) {
        start_ = selection.start;
        end_ = selection.end;
        value_ = selection.start;
    }

    [[nodiscard]] bool in_range(const long date) const {
        return date >= min_ && date <= max_;
    }

private:
    void select_one(const long date) {
        value_ = date;
        start_ = date;
        end_ = date;
    }

    // SelStart or SelEnd with MultiSelect keeps the selection within
    // MaxSelCount days; without it they select one date.
    [[nodiscard]] Outcome set_selection(const long date, const bool is_start) {
        if (!in_range(date)) {
            return {false};
        }
        if (!multi_) {
            select_one(date);
            return {};
        }
        long start = is_start ? date : start_;
        long end = is_start ? end_ : date;
        if (is_start && (end < start || end - start + 1 > most_)) {
            end = start;  // a new start leaves a one-day selection
        } else if (!is_start && end < start) {
            start = end;
        }
        if (end - start + 1 > most_) {
            return {false};
        }
        start_ = start;
        end_ = end;
        value_ = start;
        return {};
    }

    long value_;
    long start_;
    long end_;
    long min_ = calendar::first_date();
    long max_ = calendar::last_date();
    bool multi_ = false;
    long most_ = 7;
    long first_day_ = 1;
};

}  // namespace wfc::mscomctl2

#endif  // WFC_MSCOMCTL2_MONTHVIEW_HPP
