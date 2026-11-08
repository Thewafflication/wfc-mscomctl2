// Checks the behaviour of the MonthView model (wfc/mscomctl2/monthview.hpp):
// the calendar arithmetic, the selection rules, the range, the parts of Value
// and the days the calendar shows.

#include <wfc/mscomctl2/monthview.hpp>

#include <cstdio>

namespace {

int failures = 0;

void expect(const bool condition, const char* what) {
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    }
}

}  // namespace

int main() {
    using wfc::mscomctl2::DayBold;
    using wfc::mscomctl2::MonthViewModel;
    namespace calendar = wfc::mscomctl2::calendar;

    // 2000-01-01 is serial 36526, a Saturday; 1899-12-30 is serial 0.
    expect(calendar::serial(2000, 1, 1) == 36526, "the serial of 2000-01-01");
    expect(calendar::parts(36526).weekday == 7, "2000-01-01 is a Saturday");
    expect(calendar::serial(1899, 12, 30) == 0, "serial 0");
    const calendar::Parts leap = calendar::parts(calendar::serial(2024, 2, 29));
    expect(leap.year == 2024 && leap.month == 2 && leap.day == 29,
           "a leap day round trips");
    expect(calendar::days_in_month(1900, 2) == 28 &&
               calendar::days_in_month(2000, 2) == 29,
           "century leap years");
    // Week 1 is the first week of four days (as VB6 numbers them): 2000-01-01
    // is in the last week of 1999, and with weeks from Sunday 2000-01-02
    // starts week 1.
    expect(calendar::week_of_year(calendar::serial(2000, 1, 1), 1) == 52,
           "the last week of the year before");
    expect(calendar::week_of_year(calendar::serial(2000, 1, 2), 1) == 1,
           "week 1");
    expect(calendar::week_of_year(calendar::serial(2000, 1, 3), 2) == 1,
           "week 1 from Monday");
    expect(calendar::week_of_year(calendar::serial(2026, 11, 7), 1) == 44,
           "7 November 2026, as VB6 numbers it");

    const long day = calendar::serial(2024, 5, 15);  // a Wednesday
    MonthViewModel model{day};
    expect(model.value() == day && model.sel_start() == day &&
               model.sel_end() == day,
           "a new control selects its date");

    // The 42 days: May 2024 starts on a Wednesday, so a Sunday-first calendar
    // begins on Sunday April 28; from Monday it begins on April 29.
    long first = 0;
    expect(
        model.visible_day(1, first) && first == calendar::serial(2024, 4, 28),
        "the first visible day from Sunday");
    model.set_first_day(2);
    expect(
        model.visible_day(1, first) && first == calendar::serial(2024, 4, 29),
        "the first visible day from Monday");
    expect(model.visible_day(42, first), "the last visible day");
    expect(!model.visible_day(0, first) && !model.visible_day(43, first),
           "VisibleDays outside 1 to 42");
    model.set_first_day(0x10002);
    expect(model.first_day() == 2, "flag bits above the day are dropped");
    model.set_first_day(9);
    expect(model.first_day() == 1, "a bad first day is Sunday");

    // Parts of Value.
    expect(model.set_part(MonthViewModel::Part::Day, 31).valid &&
               model.value() == calendar::serial(2024, 5, 31),
           "Day 31");
    expect(model.set_part(MonthViewModel::Part::Month, 2).valid &&
               model.value() == calendar::serial(2024, 2, 29),
           "Month 2 brings Day back to the end of the month");
    expect(!model.set_part(MonthViewModel::Part::Day, 30).valid,
           "Day 30 in February is invalid");
    expect(!model.set_part(MonthViewModel::Part::Month, 13).valid,
           "Month 13 is invalid");
    expect(!model.set_part(MonthViewModel::Part::Year, 1600).valid,
           "Year 1600 is invalid");

    // The range.
    model = MonthViewModel{day};
    expect(!model.set_min(day + 1).valid || model.min() == day + 1,
           "Min above Value moves Value");
    expect(model.value() == day + 1, "Value is brought up to Min");
    expect(!model.set_max(day).valid, "Max below Min is invalid");
    expect(model.set_max(day + 5).valid, "Max above Value");
    expect(!model.set_value(day + 6).valid, "Value above Max is invalid");
    expect(!model.set_value(day).valid, "Value below Min is invalid");

    // The selection without MultiSelect is one date.
    model = MonthViewModel{day};
    expect(model.set_sel_end(day + 3).valid && model.sel_start() == day + 3 &&
               model.sel_end() == day + 3,
           "SelEnd selects one date without MultiSelect");

    // MultiSelect with MaxSelCount 3.
    model = MonthViewModel{day};
    model.set_multi_select(true);
    model.set_max_sel_count(3);
    expect(model.set_sel_end(day + 2).valid && model.sel_start() == day &&
               model.sel_end() == day + 2,
           "a selection of three days");
    expect(!model.set_sel_end(day + 3).valid, "four days are too many");
    expect(model.set_sel_start(day + 5).valid && model.sel_end() == day + 5,
           "a new start leaves a one-day selection");
    expect(model.set_sel_end(day + 1).valid && model.sel_start() == day + 1,
           "an end before the start moves the start");

    // The user's selection: checked here, applied after SelChange.
    model = MonthViewModel{day};
    auto picked = model.select(day + 2, day + 4);
    expect(picked.accepted && picked.start == day + 2 && picked.end == day + 2,
           "without MultiSelect the user selects one date");
    expect(picked.changed, "a different date is a change");
    picked = model.select(day, day);
    expect(picked.accepted && !picked.changed, "the same date is no change");
    model.set_multi_select(true);
    picked = model.select(day + 4, day + 2);
    expect(picked.accepted && picked.start == day + 2 && picked.end == day + 4,
           "a backwards selection is turned round");
    picked = model.select(day, day + 7);
    expect(!picked.accepted, "more than MaxSelCount days is refused");
    model.apply(model.select(day + 2, day + 4));
    expect(model.value() == day + 2 && model.sel_end() == day + 4,
           "apply keeps the selection");
    expect(!model.select(model.max() + 1, model.max() + 1).accepted,
           "a date outside the range is refused");

    // Bold days.
    DayBold bold;
    bold.set(day, true);
    expect(bold.is_bold(day) && !bold.is_bold(day + 1), "a bold day");
    const DayBold before = bold;
    bold.set(day, false);
    expect(!bold.is_bold(day) && !(bold == before), "a day made plain");

    if (failures == 0) {
        std::printf("monthview: the model behaves\n");
    }
    return failures == 0 ? 0 : 1;
}
