// Checks the behaviour of the UpDown model (wfc/mscomctl2/updown.hpp): the
// steps of the arrows, Wrap, the position the window reports, and the rules
// for Min, Max and Value.

#include <wfc/mscomctl2/updown.hpp>

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
    using wfc::mscomctl2::UpDownModel;

    UpDownModel model;
    expect(model.min() == 0 && model.max() == 10 && model.value() == 0 &&
               model.increment() == 1 && !model.wrap(),
           "the defaults");

    auto outcome = model.click(true);
    expect(outcome.valid && outcome.changed && outcome.up && model.value() == 1,
           "an up click steps by Increment");
    model.set_increment(4);
    outcome = model.click(true);
    expect(model.value() == 5, "Increment 4");
    model.set_increment(0);
    outcome = model.click(false);
    expect(model.value() == 4 && !outcome.up, "Increment below 1 acts as 1");

    model = UpDownModel{0, 10, 10, 1, false};
    outcome = model.click(true);
    expect(!outcome.changed && outcome.up && model.value() == 10,
           "the up arrow stops at Max");
    model = UpDownModel{0, 10, 10, 1, true};
    outcome = model.click(true);
    expect(outcome.changed && model.value() == 0, "Wrap goes round to Min");
    outcome = model.click(false);
    expect(outcome.changed && !outcome.up && model.value() == 10,
           "Wrap goes round to Max");
    model = UpDownModel{0, 10, 0, 1, false};
    outcome = model.click(false);
    expect(!outcome.changed && model.value() == 0,
           "the down arrow stops at Min");

    model = UpDownModel{0, 10, 3, 1, false};
    outcome = model.position(7);
    expect(outcome.changed && outcome.up && model.value() == 7,
           "a position above Value is the up arrow");
    outcome = model.position(2);
    expect(outcome.changed && !outcome.up && model.value() == 2,
           "a position below Value is the down arrow");
    outcome = model.position(99);
    expect(model.value() == 10, "a position is kept inside the range");

    model = UpDownModel{};
    expect(!model.set_value(11).valid && model.value() == 0,
           "Value above Max is invalid");
    expect(!model.set_value(-1).valid, "Value below Min is invalid");
    expect(model.set_value(5).changed && model.value() == 5, "Value 5");
    expect(!model.set_value(5).changed, "the same Value is no change");
    expect(!model.set_max(-1).valid && model.max() == 10,
           "Max below Min is invalid");
    outcome = model.set_max(3);
    expect(outcome.valid && outcome.changed && model.value() == 3,
           "a narrower range brings Value inside");
    outcome = model.set_min(3);
    expect(outcome.valid && !outcome.changed && model.min() == 3,
           "Min up to Value leaves it");
    expect(!model.set_min(4).valid, "Min above Max is invalid");

    if (failures == 0) {
        std::printf("updown: the model behaves\n");
    }
    return failures == 0 ? 0 : 1;
}
