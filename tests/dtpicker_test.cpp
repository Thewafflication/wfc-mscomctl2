// Checks the behaviour of the DTPicker model (wfc/mscomctl2/dtpicker.hpp) and
// the calendar helpers it uses: the value and its range, Null with CheckBox,
// the parts of the value, and the text of the test scripts.

#include <wfc/mscomctl2/dtpicker.hpp>

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
    using wfc::mscomctl2::DateTimePickerModel;
    namespace calendar = wfc::mscomctl2::calendar;
    using Part = DateTimePickerModel::Part;

    // 45000 is 2023-03-15; .5 is noon.
    const double noon = 45000.5;
    const calendar::DateTime of = calendar::datetime_parts(noon);
    expect(of.year == 2023 && of.month == 3 && of.day == 15 && of.hour == 12 &&
               of.minute == 0 && of.second == 0 && of.weekday == 4,
           "the parts of a date and time");
    const calendar::DateTime late = calendar::datetime_parts(45000.9999999);
    expect(late.day == 16 && late.hour == 0 && late.second == 0,
           "a time that rounds up to midnight is the next day");

    double value = 0;
    expect(calendar::parse_datetime("2023-03-15", "13:45:10", value) &&
               calendar::datetime_parts(value).hour == 13 &&
               calendar::datetime_parts(value).second == 10,
           "a date and time in a script");
    expect(calendar::parse_datetime("2023-03-15", "", value) && value == 45000,
           "a date in a script");
    expect(!calendar::parse_datetime("2023-02-30", "", value),
           "a date that does not exist");
    expect(!calendar::parse_datetime("2023-03-15", "24:00:00", value),
           "an hour that does not exist");

    DateTimePickerModel model{noon};
    expect(model.value() == noon && !model.is_null(), "a new control");

    auto outcome = model.set_value(45001.25);
    expect(outcome.valid && outcome.changed && model.value() == 45001.25,
           "a new value");
    outcome = model.set_value(45001.25);
    expect(outcome.valid && !outcome.changed, "the same value is no change");

    // The parts.
    expect(model.set_part(Part::Hour, 7).valid && model.parts().hour == 7,
           "Hour");
    expect(!model.set_part(Part::Hour, 24).valid, "Hour 24 is invalid");
    expect(!model.set_part(Part::Minute, 60).valid, "Minute 60 is invalid");
    expect(model.set_part(Part::Day, 31).valid && model.parts().day == 31,
           "Day 31");
    outcome = model.set_part(Part::Month, 2);
    expect(outcome.valid && model.parts().month == 2 && model.parts().day == 28,
           "Month 2 brings Day back to the end of the month");
    expect(!model.set_part(Part::Day, 30).valid,
           "Day 30 in February is invalid");
    expect(!model.set_part(Part::Year, 10000).valid, "Year 10000 is invalid");
    expect(model.set_part(Part::Second, 59).valid && model.parts().second == 59,
           "Second");
    expect(!model.set_part(Part::Second, 59).changed,
           "the same part is no change");

    // The range.
    model = DateTimePickerModel{noon};
    expect(!model.set_max(44999).valid || model.max() == 44999,
           "Max below Value");
    expect(model.value() == 44999.5,
           "Value comes down to Max, keeping its time");
    expect(model.set_min(44990).valid && model.min() == 44990, "Min");
    expect(!model.set_min(45000).valid, "Min above Max is invalid");
    expect(!model.set_value(45000.5).valid, "Value above Max is invalid");
    expect(!model.set_value(44989.5).valid, "Value below Min is invalid");
    expect(model.set_min(44999).valid && model.value() == 44999.5,
           "Min up to Value");

    // Null needs CheckBox.
    model = DateTimePickerModel{noon};
    expect(!model.set_null().valid && !model.is_null(),
           "Null without CheckBox is invalid");
    model.set_checkbox(true);
    outcome = model.set_null();
    expect(outcome.valid && outcome.changed && model.is_null(), "Null");
    expect(model.set_null().valid && !model.set_null().changed,
           "Null again is no change");
    expect(!model.set_part(Part::Day, 3).valid, "a part of Null is invalid");
    outcome = model.set_value(45002.5);
    expect(outcome.valid && outcome.changed && !model.is_null(),
           "a value after Null is a change");

    // The box: clearing keeps the value, ticking brings it back.
    model = DateTimePickerModel{noon};
    expect(!model.check(false).valid, "no box without CheckBox");
    model.set_checkbox(true);
    expect(!model.check(true).valid, "ticking a ticked box does nothing");
    expect(model.check(false).changed && model.is_null(), "clearing the box");
    expect(model.check(true).changed && model.value() == noon,
           "ticking shows the kept value");

    // Ticking a box that was never cleared by the user shows the first value.
    DateTimePickerModel::State state;
    state.value = 0;
    state.null = true;
    state.initial = 45010.25;
    state.checkbox = true;
    model = DateTimePickerModel{state};
    expect(model.check(true).changed && model.value() == 45010.25,
           "ticking shows the first value when nothing was kept");

    if (failures == 0) {
        std::printf("dtpicker: the model behaves\n");
    }
    return failures == 0 ? 0 : 1;
}
