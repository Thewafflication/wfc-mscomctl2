// Checks the behaviour of the scroll bar model (wfc/mscomctl2/scrollbar.hpp):
// the defaults, the range (Min may be above Max), the position the user moves
// to and the steps of the arrows and the track.

#include <wfc/mscomctl2/scrollbar.hpp>

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
    using wfc::mscomctl2::ScrollBarModel;

    ScrollBarModel model;
    expect(model.min() == 0 && model.max() == 32767 && model.value() == 0 &&
               model.small_change() == 1 && model.large_change() == 1,
           "the defaults");

    auto outcome = model.set_value(40);
    expect(outcome.valid && outcome.changed && model.value() == 40,
           "Value inside the range");
    outcome = model.set_value(40);
    expect(outcome.valid && !outcome.changed, "the same Value is no change");
    outcome = model.set_value(-1);
    expect(!outcome.valid && model.value() == 40, "Value below Min is 380");
    outcome = model.set_value(32768);
    expect(!outcome.valid && model.value() == 40, "Value above Max is 380");

    model = ScrollBarModel{0, 100, 50, 2, 10};
    outcome = model.step(true, false);
    expect(outcome.changed && model.value() == 52,
           "an arrow steps SmallChange");
    outcome = model.step(false, true);
    expect(model.value() == 42, "the track steps LargeChange");
    outcome = model.move_to(500);
    expect(outcome.changed && model.value() == 100, "a move stops at Max");
    outcome = model.step(true, true);
    expect(outcome.valid && !outcome.changed, "no step beyond Max");
    outcome = model.move_to(-9);
    expect(model.value() == 0, "a move stops at Min");

    // Min above Max: the bar runs backwards, the range is the same.
    model = ScrollBarModel{100, 0, 100, 5, 20};
    expect(model.low() == 0 && model.high() == 100, "Min above Max");
    outcome = model.set_value(30);
    expect(outcome.valid && model.value() == 30, "Value in a reversed range");
    outcome = model.step(true, false);
    expect(model.value() == 25, "forward is towards Max when reversed");
    outcome = model.step(false, true);
    expect(model.value() == 45, "back is towards Min when reversed");

    if (failures == 0) {
        std::puts("scrollbar model: ok");
    }
    return failures == 0 ? 0 : 1;
}
