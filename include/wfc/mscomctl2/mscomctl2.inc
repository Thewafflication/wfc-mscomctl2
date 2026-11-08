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

// The properties of the control as the library's model of its behaviour
// (wfc/mscomctl2/updown.hpp).
[[nodiscard]] wfc::mscomctl2::UpDownModel updown_model(ControlObject& self) {
    return wfc::mscomctl2::UpDownModel{
        static_cast<long>(number_property(self, "min")),
        static_cast<long>(number_property(self, "max")),
        static_cast<long>(number_property(self, "value")),
        static_cast<long>(number_property(self, "increment")),
        control_flag(self, "wrap")};
}

// The user clicks an arrow (a scripted `ui <control> up|down`), or the host
// reports the new position of the arrows (`position <value>`).
[[nodiscard]] bool updown_input(Interpreter& in, ControlObject& self,
                                const std::string& action,
                                const std::string& rest) {
    if (action != "up" && action != "down" && action != "position") {
        return true;
    }
    wfc::mscomctl2::UpDownModel model = updown_model(self);
    const auto outcome = action == "position"
                             ? model.position(std::atol(rest.c_str()))
                             : model.click(action == "up");
    if (outcome.changed) {
        self.values["value"] = Value{static_cast<Integer>(model.value())};
        updown_sync_out(in, self);
    }
    if (!raise_event(in, self, outcome.up ? "UpClick" : "DownClick", {})) {
        return false;
    }
    if (outcome.changed) {
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
    if (!buddy->get(in, property, none, held, ignored) || ignored.number != 0) {
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

// ----- MonthView
// ---------------------------------------------------------------
//
// A calendar. Value is the selected date (SelStart and SelEnd with
// MultiSelect, up to MaxSelCount days); Day, Month, Year, DayOfWeek and Week
// are the parts of Value; MinDate and MaxDate keep it in range (380 outside).
// VisibleDays(1 to 42) are the days the calendar shows for the month of
// Value, DayBold(date) marks a day. The user selecting a date raises
// SelChange(StartDate, EndDate, Cancel) and then DateClick(date); a double
// click adds DateDblClick. Dates in scripts are `yyyy-mm-dd`. The behaviour
// follows the documented control; there is no reference probe.

namespace {

struct MonthViewState final : OcxState {
    wfc::mscomctl2::DayBold bold;
};

[[nodiscard]] MonthViewState& month_state(ControlObject& self) {
    auto& slot = state_of(self).ocx;
    if (slot == nullptr) {
        slot = std::make_shared<MonthViewState>();
    }
    return static_cast<MonthViewState&>(*slot);
}

[[nodiscard]] double month_first_date() {
    return static_cast<double>(wfc::mscomctl2::calendar::first_date());
}

[[nodiscard]] double month_last_date() {
    return static_cast<double>(wfc::mscomctl2::calendar::last_date());
}

[[nodiscard]] double month_date_of(ControlObject& self, const char* name,
                                   const double fallback) {
    const auto found = self.values.find(name);
    if (found != self.values.end()) {
        if (const auto* date = std::get_if<DateValue>(&found->second)) {
            return std::floor(date->serial);
        }
    }
    return fallback;
}

// A form file keeps the date of the control as CurrentDate (a serial).
[[nodiscard]] double month_design_date(ControlObject& self) {
    const auto found = self.values.find("currentdate");
    if (found != self.values.end() &&
        !std::holds_alternative<std::string>(found->second)) {
        return std::floor(as_double(found->second));
    }
    return static_cast<double>(wfc::mscomctl2::calendar::today());
}

[[nodiscard]] double month_value(ControlObject& self) {
    return month_date_of(self, "value", month_design_date(self));
}

[[nodiscard]] double month_min(ControlObject& self) {
    return month_date_of(self, "mindate", month_first_date());
}

[[nodiscard]] double month_max(ControlObject& self) {
    return month_date_of(self, "maxdate", month_last_date());
}

[[nodiscard]] double month_sel_start(ControlObject& self) {
    return month_date_of(self, "selstart", month_value(self));
}

[[nodiscard]] double month_sel_end(ControlObject& self) {
    return month_date_of(self, "selend", month_sel_start(self));
}

[[nodiscard]] Value month_date_value(const double serial) {
    return Value{DateValue{serial}};
}

// `yyyy-mm-dd` to a date serial; false for anything else.
[[nodiscard]] bool month_parse(const std::string& text, double& serial) {
    long date = 0;
    if (!wfc::mscomctl2::calendar::parse_date(text, date)) {
        return false;
    }
    serial = static_cast<double>(date);
    return true;
}

// The properties of the control as the library's model of its behaviour
// (wfc/mscomctl2/monthview.hpp).
[[nodiscard]] wfc::mscomctl2::MonthViewModel month_model(ControlObject& self) {
    wfc::mscomctl2::MonthViewModel model{
        static_cast<long>(month_value(self)),
        static_cast<long>(month_sel_start(self)),
        static_cast<long>(month_sel_end(self)),
        static_cast<long>(month_min(self)), static_cast<long>(month_max(self))};
    model.set_multi_select(property_flag(self, "multiselect"));
    model.set_max_sel_count(property_whole(self, "maxselcount"));
    model.set_first_day(property_whole(self, "startofweek"));
    return model;
}

// Keeps what changing `member` did to the model: the member itself, and the
// selection when it moved or `member` is one of its dates.
void month_store(ControlObject& self,
                 const wfc::mscomctl2::MonthViewModel& before,
                 const wfc::mscomctl2::MonthViewModel& after,
                 const std::string& member) {
    if (member == "mindate" || member == "maxdate") {
        self.values[member] = month_date_value(static_cast<double>(
            member == "mindate" ? after.min() : after.max()));
    }
    const bool selection = member == "value" || member == "selstart" ||
                           member == "selend" || member == "day" ||
                           member == "month" || member == "year";
    if (selection || after.value() != before.value() ||
        after.sel_start() != before.sel_start() ||
        after.sel_end() != before.sel_end()) {
        self.values["value"] =
            month_date_value(static_cast<double>(after.value()));
        self.values["selstart"] =
            month_date_value(static_cast<double>(after.sel_start()));
        self.values["selend"] =
            month_date_value(static_cast<double>(after.sel_end()));
    }
}

// The first day of the week, 1 (Sunday) to 7.
[[nodiscard]] long month_first_day(ControlObject& self) {
    return month_model(self).first_day();
}

// A date argument as a whole serial (a Date, or a number).
[[nodiscard]] double month_argument(const Value& value) {
    if (const auto* date = std::get_if<DateValue>(&value)) {
        return std::floor(date->serial);
    }
    return std::floor(as_double(value));
}

[[nodiscard]] bool month_truthy(const Value& value) {
    if (const auto* flag = std::get_if<bool>(&value)) {
        return *flag;
    }
    return whole_value(value).value_or(0) != 0;
}

}  // namespace

[[nodiscard]] bool monthview_get(Interpreter& in, ControlObject& self,
                                 const std::string& member,
                                 std::vector<CallArgument>& arguments,
                                 Value& result, NativeFailure& failure) {
    const double value = month_value(self);
    const auto model = month_model(self);
    const auto parts = model.parts();
    if (member == "hittest" || member == "computecontrolsize") {
        // HitTest(x, y, Date) in pixels gives the area (MonthViewHitTestAreas)
        // and sets Date; ComputeControlSize(Rows, Columns, Width, Height) sets
        // Width and Height (twips) from the system calendar.
        const auto number = [&](const std::size_t at) {
            return at < arguments.size() ? as_double(arguments[at].value) : 0.0;
        };
        const auto give = [&](const std::size_t at, Value output) {
            if (at < arguments.size() &&
                arguments[at].byref_target != nullptr) {
                *arguments[at].byref_target = std::move(output);
            }
        };
        auto* host = self.window != ui::kNoWindow
                         ? FormSupport::runtime(in).host.get()
                         : nullptr;
        if (member == "hittest") {
            long area = 6;  // mvwNoWhere
            double date = value;
            if (host != nullptr &&
                host->month_hit(self.window, static_cast<long>(number(0)),
                                static_cast<long>(number(1)), area, date) &&
                area != 6) {
                give(2, month_date_value(date));
            }
            result = Value{static_cast<Int16>(area)};
        } else {
            double width = 0;
            double height = 0;
            if (host != nullptr) {
                host->month_size(self.window, static_cast<long>(number(0)),
                                 static_cast<long>(number(1)), width, height);
            }
            give(2, Value{static_cast<float>(width)});
            give(3, Value{static_cast<float>(height)});
            result = Value{Empty{}};
        }
        return true;
    }
    if (member == "value") {
        result = month_date_value(value);
    } else if (member == "selstart") {
        result = month_date_value(month_sel_start(self));
    } else if (member == "selend") {
        result = month_date_value(month_sel_end(self));
    } else if (member == "mindate") {
        result = month_date_value(month_min(self));
    } else if (member == "maxdate") {
        result = month_date_value(month_max(self));
    } else if (member == "day") {
        result = Value{static_cast<Int16>(parts.day)};
    } else if (member == "month") {
        result = Value{static_cast<Integer>(parts.month)};  // MonthConstants
    } else if (member == "year") {
        result = Value{static_cast<Int16>(parts.year)};
    } else if (member == "dayofweek") {
        result = Value{static_cast<Integer>(parts.weekday)};  // DayConstants
    } else if (member == "week") {
        result = Value{static_cast<Int16>(model.week())};
    } else if (member == "visibledays") {
        const long index =
            arguments.empty()
                ? 0
                : static_cast<long>(as_double(arguments[0].value));
        long date = 0;
        if (!model.visible_day(index, date)) {
            failure = NativeFailure{380, "Invalid property value"};
            return true;
        }
        result = month_date_value(static_cast<double>(date));
    } else if (member == "daybold") {
        const double day =
            arguments.empty() ? 0.0 : month_argument(arguments[0].value);
        result = Value{month_state(self).bold.is_bold(static_cast<long>(day))};
    } else {
        return false;
    }
    return true;
}

[[nodiscard]] bool monthview_put(Interpreter& in, ControlObject& self,
                                 const std::string& member,
                                 std::vector<CallArgument>& arguments,
                                 const Value& value, NativeFailure& failure) {
    const auto invalid = [&] {
        failure = NativeFailure{380, "Invalid property value"};
        return true;
    };
    if (member == "daybold") {
        if (arguments.empty()) {
            return invalid();
        }
        const long day = static_cast<long>(month_argument(arguments[0].value));
        month_state(self).bold.set(day, month_truthy(value));
        return true;
    }
    if (member == "value" || member == "selstart" || member == "selend" ||
        member == "mindate" || member == "maxdate") {
        Value converted = value;
        if (!coerce_row(in, self, member, converted, failure)) {
            return true;
        }
        const auto* date = std::get_if<DateValue>(&converted);
        if (date == nullptr) {
            return invalid();
        }
        const long wanted = static_cast<long>(std::floor(date->serial));
        auto model = month_model(self);
        const auto before = model;
        const auto outcome = member == "mindate"   ? model.set_min(wanted)
                             : member == "maxdate" ? model.set_max(wanted)
                             : member == "value"   ? model.set_value(wanted)
                             : member == "selstart"
                                 ? model.set_sel_start(wanted)
                                 : model.set_sel_end(wanted);
        if (!outcome.valid) {
            return invalid();
        }
        month_store(self, before, model, member);
        return true;
    }
    if (member == "week" || member == "dayofweek") {
        // Moves Value to that week of its year, or to that day of its week.
        Value converted = value;
        if (!coerce_row(in, self, member, converted, failure)) {
            return true;
        }
        const long number = static_cast<long>(as_double(converted));
        auto model = month_model(self);
        const auto before = model;
        const long date = model.value();
        const long moved = member == "week"
                               ? date + (number - model.week()) * 7
                               : date + (number - model.parts().weekday);
        if ((member == "week" && (number < 1 || number > 53)) ||
            (member == "dayofweek" && (number < 1 || number > 7)) ||
            !model.set_value(moved).valid) {
            return invalid();
        }
        month_store(self, before, model, member);
        return true;
    }
    if (member == "day" || member == "month" || member == "year") {
        Value converted = value;
        if (!coerce_row(in, self, member, converted, failure)) {
            return true;
        }
        const long number = static_cast<long>(as_double(converted));
        auto model = month_model(self);
        const auto before = model;
        const auto outcome = model.set_part(
            member == "year"    ? wfc::mscomctl2::MonthViewModel::Part::Year
            : member == "month" ? wfc::mscomctl2::MonthViewModel::Part::Month
                                : wfc::mscomctl2::MonthViewModel::Part::Day,
            number);
        if (!outcome.valid) {
            return invalid();
        }
        month_store(self, before, model, member);
        return true;
    }
    return false;
}

namespace {

// The user selected `start` to `end` (one date unless MultiSelect): SelChange
// runs first and can cancel, then DateClick.
[[nodiscard]] bool month_user_select(Interpreter& in, ControlObject& self,
                                     double start, double end,
                                     const bool double_click) {
    const auto model = month_model(self);
    const auto selection =
        model.select(static_cast<long>(start), static_cast<long>(end));
    if (!selection.accepted) {
        return true;
    }
    start = static_cast<double>(selection.start);
    end = static_cast<double>(selection.end);
    const bool changed = selection.changed;
    if (changed) {
        std::vector<Value> arguments{month_date_value(start),
                                     month_date_value(end), Value{false}};
        const auto form = self.form.lock();
        if (form != nullptr &&
            !FormSupport::raise(in, *form, self.name, self.index, "selchange",
                                arguments, FormSupport::offset_of(in))) {
            return false;
        }
        if (!month_truthy(arguments[2])) {
            self.values["selstart"] = month_date_value(start);
            self.values["selend"] = month_date_value(end);
            self.values["value"] = month_date_value(start);
        }
    }
    if (!raise_event(in, self, "DateClick", {month_date_value(start)})) {
        return false;
    }
    return !double_click ||
           raise_event(in, self, "DateDblClick", {month_date_value(start)});
}

// The calendar shows `count` days from `start`: GetDayBold(StartDate, Count,
// State()) runs with State a Boolean array (0 to Count - 1) and the days it
// sets are bold from then on, the others of that stretch are not.
[[nodiscard]] bool month_get_day_bold(Interpreter& in, ControlObject& self,
                                      const double start, const long count) {
    if (count < 1 || count > 400) {
        return true;
    }
    ArrayValue state;
    state.lower_bound = 0;
    state.elements.assign(static_cast<std::size_t>(count), Value{false});
    state.element_type_index = Value{false}.index();
    std::vector<Value> arguments{month_date_value(start),
                                 Value{static_cast<Int16>(count)},
                                 Value{std::move(state)}};
    const auto form = self.form.lock();
    if (form == nullptr ||
        !FormSupport::raise(in, *form, self.name, self.index, "getdaybold",
                            arguments, FormSupport::offset_of(in))) {
        return false;
    }
    const auto* result = std::get_if<ArrayValue>(&arguments[2]);
    if (result == nullptr) {
        return true;
    }
    auto& bold = month_state(self).bold;
    const wfc::mscomctl2::DayBold before = bold;
    for (long at = 0; at < count; ++at) {
        const long day = static_cast<long>(start) + at;
        if (static_cast<std::size_t>(at) < result->elements.size() &&
            month_truthy(result->elements[static_cast<std::size_t>(at)])) {
            bold.set(day, true);
        } else {
            bold.set(day, false);
        }
    }
    if (bold != before) {
        // The request came from the host, which still has the old days.
        const bool echoed = self.comctl != nullptr && self.comctl->from_host;
        ComctlSupport::from_host(self, false);
        comctl_push(in, self);
        ComctlSupport::from_host(self, echoed);
    }
    return true;
}

}  // namespace

// Scripted input: `click|dblclick yyyy-mm-dd`, `select yyyy-mm-dd yyyy-mm-dd`,
// `getdaybold yyyy-mm-dd count`.
[[nodiscard]] bool monthview_input(Interpreter& in, ControlObject& self,
                                   const std::string& action,
                                   const std::string& rest) {
    std::istringstream words(rest);
    std::string first;
    std::string second;
    words >> first >> second;
    double start = 0;
    double end = 0;
    if (!month_parse(first, start)) {
        return true;
    }
    if (action == "getdaybold") {
        return month_get_day_bold(
            in, self, start, second.empty() ? 0 : std::atol(second.c_str()));
    }
    if (action == "click" || action == "dblclick") {
        return month_user_select(in, self, start, start, action == "dblclick");
    }
    if (action == "select" && month_parse(second, end)) {
        return month_user_select(in, self, start, end, false);
    }
    return true;
}

// What the host draws: the dates, the range and how the calendar is shown.
// The colours of a calendar set in the form or by code: the property names of
// the control with the MCSC_* index of each (background 0, text 1, title
// background 2, title text 3, month background 4, trailing text 5).
void calendar_colors(ControlObject& self,
                     std::initializer_list<std::pair<const char*, int>> names,
                     ui::CommonSnapshot& snapshot) {
    for (const auto& [name, index] : names) {
        const auto found = self.values.find(name);
        if (found != self.values.end() &&
            !std::holds_alternative<std::string>(found->second)) {
            snapshot.calendar_colors.emplace_back(
                index, static_cast<long>(as_double(found->second)));
        }
    }
}

void monthview_snapshot(ControlObject& self, ui::CommonSnapshot& snapshot) {
    snapshot.date_value = month_value(self);
    snapshot.date_start = month_sel_start(self);
    snapshot.date_end = month_sel_end(self);
    snapshot.date_min = month_min(self);
    snapshot.date_max = month_max(self);
    snapshot.show_today = property_flag(self, "showtoday");
    snapshot.week_numbers = property_flag(self, "showweeknumbers");
    snapshot.multi_select = property_flag(self, "multiselect");
    snapshot.first_day = month_first_day(self);
    snapshot.max_select = property_whole(self, "maxselcount");
    calendar_colors(self,
                    {{"backcolor", 0},
                     {"forecolor", 1},
                     {"titlebackcolor", 2},
                     {"titleforecolor", 3},
                     {"monthbackcolor", 4},
                     {"trailingforecolor", 5}},
                    snapshot);
    for (const long day : month_state(self).bold.days()) {
        snapshot.bold_days.push_back(static_cast<double>(day));
    }
}

// ----- DTPicker
// ------------------------------------------------------------------
//
// A date and time picker. Value is a Date (a Null Variant while CheckBox is
// True and the box is cleared); Format (0 long date, 1 short date, 2 time, 3
// CustomFormat) says how it is shown; UpDown replaces the drop-down calendar
// with spin arrows. Day, Month, Year, Hour, Minute, Second and DayOfWeek are
// the parts of Value (Null when it is Null). MinDate and MaxDate keep it in
// range (380 outside). Change is raised when Value changes, by code or by the
// user; DropDown and CloseUp bracket the calendar. In scripts: `ui <control>
// value yyyy-mm-dd[ hh:mm:ss]`, `ui <control> check on|off`, `ui <control>
// dropdown` and `ui <control> closeup`. The behaviour follows the documented
// control; there is no reference probe.

namespace {

// Now, as a date serial with the time of day.
[[nodiscard]] double picker_now() {
    return wfc::mscomctl2::calendar::now();
}

// The date of a form file (CurrentDate, with the time as the fraction), or now.
[[nodiscard]] double picker_design_date(ControlObject& self) {
    const auto found = self.values.find("currentdate");
    if (found != self.values.end() &&
        !std::holds_alternative<std::string>(found->second)) {
        return as_double(found->second);
    }
    return picker_now();
}

[[nodiscard]] bool picker_is_null(ControlObject& self) {
    const auto found = self.values.find("value");
    return found != self.values.end() &&
           std::holds_alternative<Null>(found->second);
}

[[nodiscard]] double picker_value(ControlObject& self) {
    const auto found = self.values.find("value");
    if (found != self.values.end()) {
        if (const auto* date = std::get_if<DateValue>(&found->second)) {
            return date->serial;
        }
    }
    return picker_design_date(self);
}

// The Format property: a form file keeps flag bits above the number.
[[nodiscard]] long picker_format(ControlObject& self) {
    const long format = property_whole(self, "format") & 0xFFFF;
    return format >= 0 && format <= 3 ? format : 0;
}

// The properties of the control as the library's model of its behaviour
// (wfc/mscomctl2/dtpicker.hpp).
[[nodiscard]] wfc::mscomctl2::DateTimePickerModel picker_model(
    ControlObject& self) {
    wfc::mscomctl2::DateTimePickerModel::State state;
    state.initial = picker_design_date(self);
    state.value = picker_value(self);
    state.null = picker_is_null(self);
    state.min =
        static_cast<long>(month_date_of(self, "mindate", month_first_date()));
    state.max =
        static_cast<long>(month_date_of(self, "maxdate", month_last_date()));
    state.checkbox = property_flag(self, "checkbox");
    const auto last = self.values.find("lastvalue");
    if (last != self.values.end()) {
        if (const auto* date = std::get_if<DateValue>(&last->second)) {
            state.has_last = true;
            state.last = date->serial;
        }
    }
    return wfc::mscomctl2::DateTimePickerModel{state};
}

// Keeps what changing `member` did to the model.
void picker_store(ControlObject& self,
                  const wfc::mscomctl2::DateTimePickerModel& before,
                  const wfc::mscomctl2::DateTimePickerModel& after,
                  const std::string& member) {
    if (member == "mindate" || member == "maxdate") {
        self.values[member] = month_date_value(static_cast<double>(
            member == "mindate" ? after.min() : after.max()));
        if (after.is_null() || after.value() == before.value()) {
            return;  // the value stays as it is
        }
    }
    self.values["value"] =
        after.is_null() ? Value{Null{}} : month_date_value(after.value());
    if (after.state().has_last) {
        self.values["lastvalue"] = month_date_value(after.state().last);
    }
}

}  // namespace

[[nodiscard]] bool dtpicker_get(Interpreter& in, ControlObject& self,
                                const std::string& member, Value& result) {
    static_cast<void>(in);
    if (member == "value") {
        result = picker_is_null(self) ? Value{Null{}}
                                      : month_date_value(picker_value(self));
        return true;
    }
    if (member == "format") {
        result = Value{static_cast<Integer>(picker_format(self))};
        return true;
    }
    if (member == "mindate" || member == "maxdate") {
        result = month_date_value(
            member == "mindate"
                ? month_date_of(self, "mindate", month_first_date())
                : month_date_of(self, "maxdate", month_last_date()));
        return true;
    }
    if (member == "day" || member == "month" || member == "year" ||
        member == "hour" || member == "minute" || member == "second" ||
        member == "dayofweek") {
        if (picker_is_null(self)) {
            result = Value{Null{}};
            return true;
        }
        const auto parts = picker_model(self).parts();
        const long number = member == "day"      ? parts.day
                            : member == "month"  ? parts.month
                            : member == "year"   ? parts.year
                            : member == "hour"   ? parts.hour
                            : member == "minute" ? parts.minute
                            : member == "second" ? parts.second
                                                 : parts.weekday;
        result = Value{static_cast<Int16>(number)};
        return true;
    }
    return false;
}

[[nodiscard]] bool dtpicker_put(Interpreter& in, ControlObject& self,
                                const std::string& member, const Value& value,
                                NativeFailure& failure) {
    const auto invalid = [&] {
        failure = NativeFailure{380, "Invalid property value"};
        return true;
    };
    if (member == "value") {
        auto model = picker_model(self);
        const auto before = model;
        if (std::holds_alternative<Null>(value)) {
            const auto outcome = model.set_null();
            if (!outcome.valid) {
                return invalid();
            }
            picker_store(self, before, model, member);
            return !outcome.changed || raise_event(in, self, "Change", {});
        }
        Value converted = value;
        if (!coerce_property(in, converted, "Date", failure)) {
            return true;
        }
        const auto* date = std::get_if<DateValue>(&converted);
        if (date == nullptr) {
            return invalid();
        }
        const auto outcome = model.set_value(date->serial);
        if (!outcome.valid) {
            return invalid();
        }
        picker_store(self, before, model, member);
        return !outcome.changed || raise_event(in, self, "Change", {});
    }
    if (member == "mindate" || member == "maxdate") {
        Value converted = value;
        if (!coerce_property(in, converted, "Date", failure)) {
            return true;
        }
        const auto* date = std::get_if<DateValue>(&converted);
        if (date == nullptr) {
            return invalid();
        }
        auto model = picker_model(self);
        const auto before = model;
        const long wanted = static_cast<long>(std::floor(date->serial));
        const auto outcome =
            member == "mindate" ? model.set_min(wanted) : model.set_max(wanted);
        if (!outcome.valid) {
            return invalid();
        }
        picker_store(self, before, model, member);
        return true;
    }
    if (member == "day" || member == "month" || member == "year" ||
        member == "hour" || member == "minute" || member == "second") {
        if (picker_is_null(self)) {
            return invalid();
        }
        Value converted = value;
        if (!coerce_property(in, converted, "Long", failure)) {
            return true;
        }
        using Part = wfc::mscomctl2::DateTimePickerModel::Part;
        auto model = picker_model(self);
        const auto before = model;
        const auto outcome =
            model.set_part(member == "year"     ? Part::Year
                           : member == "month"  ? Part::Month
                           : member == "day"    ? Part::Day
                           : member == "hour"   ? Part::Hour
                           : member == "minute" ? Part::Minute
                                                : Part::Second,
                           static_cast<long>(as_double(converted)));
        if (!outcome.valid) {
            return invalid();
        }
        picker_store(self, before, model, member);
        return !outcome.changed || raise_event(in, self, "Change", {});
    }
    return false;
}

namespace {

// What the program's callback events gave for each callback field: the text
// (Format) and the width (FormatSize).
struct PickerState final : OcxState {
    std::map<std::string, std::pair<std::string, long>> fields;
};

[[nodiscard]] PickerState& picker_state(ControlObject& self) {
    auto& slot = state_of(self).ocx;
    if (slot == nullptr) {
        slot = std::make_shared<PickerState>();
    }
    return static_cast<PickerState&>(*slot);
}

// Format(CallbackField, FormattedString), FormatSize(CallbackField, Size) and
// CallbackKeyDown(KeyCode, Shift, CallbackField, CallbackDate): the program
// answers through the By-Ref arguments.
[[nodiscard]] bool picker_callback(Interpreter& in, ControlObject& self,
                                   const std::string& event,
                                   std::vector<Value>& arguments) {
    const auto form = self.form.lock();
    return form == nullptr ||
           FormSupport::raise(in, *form, self.name, self.index, event,
                              arguments, FormSupport::offset_of(in));
}

// Pushes the new answers to the host, which asked from its notification.
void picker_push(Interpreter& in, ControlObject& self) {
    const bool echoed = self.comctl != nullptr && self.comctl->from_host;
    ComctlSupport::from_host(self, false);
    comctl_push(in, self);
    ComctlSupport::from_host(self, echoed);
}

}  // namespace

// Scripted input: the user picks a value, ticks the box, opens the calendar.
// The host also sends `format <field> <date> <time>`, `formatsize <field>` and
// `keydown <field> <key> <shift> <date> <time>` for the callback fields.
[[nodiscard]] bool dtpicker_input(Interpreter& in, ControlObject& self,
                                  const std::string& action,
                                  const std::string& rest) {
    std::istringstream words(rest);
    std::string first;
    std::string second;
    words >> first >> second;
    if (action == "dropdown") {
        return raise_event(in, self, "DropDown", {});
    }
    if (action == "format") {
        std::vector<Value> arguments{Value{first}, Value{std::string{}}};
        if (!picker_callback(in, self, "format", arguments)) {
            return false;
        }
        auto& slot = picker_state(self).fields[first];
        const std::string text = FormSupport::text_of(arguments[1]);
        if (text != slot.first) {
            slot.first = text;
            picker_push(in, self);
        }
        return true;
    }
    if (action == "formatsize") {
        std::vector<Value> arguments{Value{first},
                                     Value{static_cast<Int16>(0)}};
        if (!picker_callback(in, self, "formatsize", arguments)) {
            return false;
        }
        auto& slot = picker_state(self).fields[first];
        const long size = static_cast<long>(as_double(arguments[1]));
        if (size != slot.second) {
            slot.second = size;
            picker_push(in, self);
        }
        return true;
    }
    if (action == "keydown") {
        std::string key;
        std::string shift;
        std::string time;
        words.clear();
        words.str(rest);
        words >> first >> key >> shift >> second >> time;
        double serial = 0;
        if (!wfc::mscomctl2::calendar::parse_datetime(second, time, serial)) {
            return true;
        }
        std::vector<Value> arguments{
            Value{static_cast<Int16>(std::atol(key.c_str()))},
            Value{static_cast<Int16>(std::atol(shift.c_str()))}, Value{first},
            month_date_value(serial)};
        if (!picker_callback(in, self, "callbackkeydown", arguments)) {
            return false;
        }
        const auto* date = std::get_if<DateValue>(&arguments[3]);
        auto model = picker_model(self);
        const auto before = model;
        if (date != nullptr && date->serial != serial &&
            model.set_value(date->serial).valid) {
            picker_store(self, before, model, "value");
            return raise_event(in, self, "Change", {});
        }
        return true;
    }
    if (action == "closeup") {
        return raise_event(in, self, "CloseUp", {});
    }
    if (action == "check") {
        const bool on = first == "on" || first == "1" || first == "true";
        auto model = picker_model(self);
        const auto before = model;
        if (!model.check(on).valid) {
            return true;
        }
        picker_store(self, before, model, "value");
        return raise_event(in, self, "Change", {});
    }
    if (action == "value") {
        double serial = 0;
        if (!wfc::mscomctl2::calendar::parse_datetime(first, second, serial)) {
            return true;
        }
        auto model = picker_model(self);
        const auto before = model;
        const auto outcome = model.set_value(serial);
        if (!outcome.valid) {
            return true;
        }
        picker_store(self, before, model, "value");
        return !outcome.changed || raise_event(in, self, "Change", {});
    }
    return true;
}

// What the host draws.
void dtpicker_snapshot(ControlObject& self, ui::CommonSnapshot& snapshot) {
    calendar_colors(self,
                    {{"calendarforecolor", 1},
                     {"calendartitlebackcolor", 2},
                     {"calendartitleforecolor", 3},
                     {"calendarbackcolor", 4},
                     {"calendartrailingforecolor", 5}},
                    snapshot);
    snapshot.date_value = picker_is_null(self) ? 0.0 : picker_value(self);
    snapshot.date_min = month_date_of(self, "mindate", month_first_date());
    snapshot.date_max = month_date_of(self, "maxdate", month_last_date());
    snapshot.style = picker_format(self);
    snapshot.text = property_text(self, "customformat");
    snapshot.checkboxes = property_flag(self, "checkbox");
    snapshot.select_range = picker_is_null(self);  // the box is cleared
    for (const auto& [field, answer] : picker_state(self).fields) {
        ui::CommonItem item;
        item.text = field;
        item.key = answer.first;
        item.width = answer.second;
        snapshot.items.push_back(item);
    }
}

// ----- Animation
// -----------------------------------------------------------------
//
// Plays a silent AVI clip. Open(file) loads it (53 when the file does not
// exist, 5 when it is not an AVI file); with AutoPlay it starts at once.
// Play(RepeatCount, StartFrame, EndFrame) starts it (RepeatCount -1, the
// default, repeats for ever; frames count from 0 and EndFrame -1 is the last),
// Stop halts it and Close unloads the file. Click and DblClick are the
// ordinary mouse events. Center and BackStyle only change how the clip is
// drawn. The behaviour follows the documented control; there is no reference
// probe.

namespace {

// 0 when `path` is an existing file that starts like an AVI file, else the
// error to raise.
[[nodiscard]] Integer animation_check(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.good()) {
        return 53;
    }
    char head[12] = {};
    file.read(head, sizeof(head));
    if (file.gcount() < 12 || std::memcmp(head, "RIFF", 4) != 0 ||
        std::memcmp(head + 8, "AVI ", 4) != 0) {
        return 5;
    }
    return 0;
}

void animation_host(Interpreter& in, ControlObject& self,
                    const std::string& action, const std::string& path,
                    const long repeat, const long first, const long last) {
    if (self.window != ui::kNoWindow) {
        FormSupport::runtime(in).host->animate(self.window, action, path,
                                               repeat, first, last);
    }
}

}  // namespace

// The methods of the control (called like read members).
[[nodiscard]] bool animation_get(Interpreter& in, ControlObject& self,
                                 const std::string& member,
                                 std::vector<CallArgument>& arguments,
                                 Value& result, NativeFailure& failure) {
    if (member == "open") {
        const std::string path = arguments.empty()
                                     ? std::string{}
                                     : FormSupport::text_of(arguments[0].value);
        result = Value{Empty{}};
        if (const Integer problem = animation_check(path); problem != 0) {
            failure = NativeFailure{
                problem, problem == 53 ? "File not found"
                                       : "Invalid procedure call or argument"};
            return true;
        }
        self.values["_file"] = Value{path};
        self.values["_playing"] = Value{false};
        animation_host(in, self, "open", path, 0, 0, 0);
        if (property_flag(self, "autoplay")) {
            self.values["_playing"] = Value{true};
            animation_host(in, self, "play", path, -1, 0, -1);
        }
        return true;
    }
    if (member == "play" || member == "stop" || member == "close") {
        result = Value{Empty{}};
        if (member == "close") {
            self.values.erase("_file");
            self.values["_playing"] = Value{false};
            animation_host(in, self, "close", "", 0, 0, 0);
        } else if (member == "stop") {
            self.values["_playing"] = Value{false};
            animation_host(in, self, "stop", "", 0, 0, 0);
        } else if (self.values.contains("_file")) {
            const auto number = [&](const std::size_t at, const long fallback) {
                return at < arguments.size() && !arguments[at].omitted &&
                               !std::holds_alternative<Empty>(
                                   arguments[at].value)
                           ? static_cast<long>(as_double(arguments[at].value))
                           : fallback;
            };
            self.values["_playing"] = Value{true};
            animation_host(in, self, "play", "", number(0, -1), number(1, 0),
                           number(2, -1));
        }
        return true;
    }
    return false;
}
