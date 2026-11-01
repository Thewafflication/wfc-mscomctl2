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
        updown_sync_out(in, self);
    }
    if (!raise_event(in, self, up ? "UpClick" : "DownClick", {})) {
        return false;
    }
    if (wanted != current) {
        return raise_event(in, self, "Change", {});
    }
    return true;
}

// ----- The buddy of an UpDown -----------------------------------------------
//
// BuddyControl names (or is) a control whose BuddyProperty follows Value while
// SyncBuddy is True: the arrows write Value to the buddy, and reading Value
// takes the number the buddy holds. With AutoBuddy the buddy is the control
// before the UpDown in the form's creation order. In a .frm file BuddyControl
// is the buddy's name and BuddyProperty a dispatch id, which stands for the
// buddy's default property.

namespace {

[[nodiscard]] ControlObject* updown_buddy(ControlObject& self) {
    const auto form = self.form.lock();
    if (form == nullptr) {
        return nullptr;
    }
    const auto found = self.values.find("buddycontrol");
    if (found != self.values.end()) {
        if (ControlObject* object = control_of(found->second)) {
            return object;
        }
        if (const auto* name = std::get_if<std::string>(&found->second);
            name != nullptr && !name->empty()) {
            return FormSupport::control_named(*form, lower_text(*name), -1);
        }
    }
    if (!control_flag(self, "autobuddy")) {
        return nullptr;
    }
    ControlObject* previous = nullptr;
    FormSupport::for_each_control(*form, [&](ControlObject& candidate) {
        if (candidate.order < self.order && candidate.focusable() &&
            (previous == nullptr || candidate.order > previous->order)) {
            previous = &candidate;
        }
    });
    return previous;
}

// The buddy's property as a lower-case name: BuddyProperty when it is a name,
// the default property of the buddy otherwise (also for a dispatch id).
[[nodiscard]] std::string updown_buddy_property(ControlObject& self,
                                                const ControlObject& buddy) {
    const auto found = self.values.find("buddyproperty");
    if (found != self.values.end()) {
        if (const auto* name = std::get_if<std::string>(&found->second);
            name != nullptr && !name->empty()) {
            char* end = nullptr;
            std::strtod(name->c_str(), &end);
            if (end == name->c_str() || *end != '\0') {
                return lower_text(*name);
            }
        }
    }
    return default_property_of(buddy.class_key);
}

}  // namespace

// The arrows changed Value: the buddy follows.
void updown_sync_out(Interpreter& in, ControlObject& self) {
    if (!control_flag(self, "syncbuddy")) {
        return;
    }
    ControlObject* buddy = updown_buddy(self);
    if (buddy == nullptr) {
        return;
    }
    const std::string property = updown_buddy_property(self, *buddy);
    if (property.empty()) {
        return;
    }
    const long value = static_cast<long>(number_property(self, "value"));
    const Value wanted = property == "text" || property == "caption"
                             ? Value{std::to_string(value)}
                             : Value{static_cast<Integer>(value)};
    NativeFailure ignored;
    std::vector<CallArgument> none;
    static_cast<void>(buddy->put(in, property, none, wanted, ignored));
}

// Reading Value: the number the buddy holds, kept inside Min and Max.
void updown_sync_in(Interpreter& in, ControlObject& self) {
    if (!control_flag(self, "syncbuddy")) {
        return;
    }
    ControlObject* buddy = updown_buddy(self);
    if (buddy == nullptr) {
        return;
    }
    const std::string property = updown_buddy_property(self, *buddy);
    if (property.empty()) {
        return;
    }
    NativeFailure ignored;
    std::vector<CallArgument> none;
    Value held;
    if (!buddy->get(in, property, none, held, ignored) ||
        ignored.number != 0) {
        return;
    }
    double number = 0;
    if (const auto* text = std::get_if<std::string>(&held)) {
        char* end = nullptr;
        number = std::strtod(text->c_str(), &end);
        if (end == text->c_str() || *end != '\0') {
            return;  // not a number: Value stays
        }
    } else {
        number = as_double(held);
    }
    const double low = number_property(self, "min");
    const double high = number_property(self, "max");
    self.values["value"] =
        Value{static_cast<Integer>(std::min(std::max(number, low), high))};
}

[[nodiscard]] bool updown_get(Interpreter& in, ControlObject& self,
                              const std::string& member) {
    if (member == "value") {
        updown_sync_in(in, self);
    }
    return false;  // the stored value is read by the common code
}

[[nodiscard]] bool updown_put(Interpreter& in, ControlObject& self,
                              const std::string& member, const Value& value) {
    if (member != "buddycontrol" && member != "syncbuddy" &&
        member != "buddyproperty" && member != "autobuddy") {
        return false;
    }
    self.values[member] = value;
    updown_sync_out(in, self);
    return true;
}

// The form has loaded: a synchronized buddy shows its UpDown's value.
void comctl_form_loaded(Interpreter& in, InstanceData& form) {
    FormSupport::for_each_control(form, [&](ControlObject& control) {
        if (control.class_key == "updown") {
            updown_sync_out(in, control);
        }
    });
}
