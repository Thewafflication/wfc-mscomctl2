// The MSComCtl2 controls (mscomct2.ocx): Animation, UpDown, MonthView,
// DTPicker and FlatScrollBar, one at a time. Included once by forms.cpp after
// the other OCX libraries, inside namespace wfc::detail. Internal to the WFC
// evaluator; not part of the public API.
//
// Done so far: UpDown. The class tables are in mscomctl2_tables.cpp; the
// defaults and the shared range handling (Value, Min, Max and Change) are in
// mscomctl.inc, with ProgressBar and Slider.

// ----- UpDown ----------------------------------------------------------------
//
// A pair of arrows that steps Value by Increment between Min and Max. A click
// on the up arrow raises UpClick, one on the down arrow DownClick, and each
// raises Change when the value changed. With Wrap the value goes round from
// Max to Min and back. The behaviour follows the documented control (there is
// no reference probe of the Microsoft control yet, see COMPATIBILITY.md).

// The user clicks an arrow (a scripted `ui <control> up|down`), or the host
// reports the new position of the arrows (`position <value>`).
[[nodiscard]] bool updown_input(Interpreter& in, ControlObject& self,
                                const std::string& action,
                                const std::string& rest) {
    if (action != "up" && action != "down" && action != "position") {
        return true;
    }
    const long minimum = static_cast<long>(number_property(self, "min"));
    const long maximum = static_cast<long>(number_property(self, "max"));
    const long current = static_cast<long>(number_property(self, "value"));
    long wanted = current;
    bool up = action == "up";
    if (action == "position") {
        wanted = std::min(std::max(std::atol(rest.c_str()), minimum), maximum);
        up = wanted >= current;
    } else {
        const long step = std::max(
            1L, static_cast<long>(number_property(self, "increment")));
        wanted = up ? current + step : current - step;
        if (wanted > maximum) {
            wanted = control_flag(self, "wrap") ? minimum : maximum;
        } else if (wanted < minimum) {
            wanted = control_flag(self, "wrap") ? maximum : minimum;
        }
    }
    if (wanted != current) {
        self.values["value"] = Value{static_cast<Integer>(wanted)};
    }
    if (!raise_event(in, self, up ? "UpClick" : "DownClick", {})) {
        return false;
    }
    if (wanted != current) {
        return raise_event(in, self, "Change", {});
    }
    return true;
}
