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

// ----- MonthView ---------------------------------------------------------------
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
    std::set<long> bold;
};

[[nodiscard]] MonthViewState& month_state(ControlObject& self) {
    auto& slot = state_of(self).ocx;
    if (slot == nullptr) {
        slot = std::make_shared<MonthViewState>();
    }
    return static_cast<MonthViewState&>(*slot);
}

[[nodiscard]] double month_first_date() {
    return date_serial(1601, 1, 1);
}

[[nodiscard]] double month_last_date() {
    return date_serial(9999, 12, 31);
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

// Today's date (the local one), as a date serial.
[[nodiscard]] double month_today() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    return date_serial(local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
}

// A form file keeps the date of the control as CurrentDate (a serial).
[[nodiscard]] double month_design_date(ControlObject& self) {
    const auto found = self.values.find("currentdate");
    if (found != self.values.end() &&
        !std::holds_alternative<std::string>(found->second)) {
        return std::floor(as_double(found->second));
    }
    return month_today();
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
    char* end = nullptr;
    const long year = std::strtol(text.c_str(), &end, 10);
    if (*end != '-') {
        return false;
    }
    const long month = std::strtol(end + 1, &end, 10);
    if (*end != '-') {
        return false;
    }
    const long day = std::strtol(end + 1, &end, 10);
    if (*end != '\0' || year < 1601 || year > 9999 || month < 1 ||
        month > 12 || day < 1 || day > days_in_month(year, month)) {
        return false;
    }
    serial = date_serial(year, month, day);
    return true;
}

[[nodiscard]] bool month_in_range(ControlObject& self, const double serial) {
    return serial >= month_min(self) && serial <= month_max(self);
}

// Makes `serial` the only selected date.
void month_select_one(ControlObject& self, const double serial) {
    self.values["value"] = month_date_value(serial);
    self.values["selstart"] = month_date_value(serial);
    self.values["selend"] = month_date_value(serial);
}

// The first day of the week, 1 (Sunday) to 7. A form file stores the value
// with flag bits above the day.
[[nodiscard]] long month_first_day(ControlObject& self) {
    const long day = property_whole(self, "startofweek") & 0xFFFF;
    return day >= 1 && day <= 7 ? day : 1;
}

[[nodiscard]] double month_first_visible(ControlObject& self) {
    const auto parts = split_date(month_value(self));
    const double first_of_month = date_serial(parts.year, parts.month, 1);
    const long first_day = month_first_day(self);
    const auto weekday = split_date(first_of_month).weekday;
    return first_of_month - static_cast<double>((weekday - first_day + 7) % 7);
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
    static_cast<void>(in);
    const double value = month_value(self);
    const auto parts = split_date(value);
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
        result = Value{static_cast<Int16>(parts.month)};
    } else if (member == "year") {
        result = Value{static_cast<Int16>(parts.year)};
    } else if (member == "dayofweek") {
        result = Value{static_cast<Int16>(parts.weekday)};
    } else if (member == "week") {
        result = Value{static_cast<Int16>(
            vb_week_of_year(value, month_first_day(self), 1))};
    } else if (member == "visibledays") {
        const long index =
            arguments.empty() ? 0
                              : static_cast<long>(as_double(arguments[0].value));
        if (index < 1 || index > 42) {
            failure = NativeFailure{380, "Invalid property value"};
            return true;
        }
        result = month_date_value(month_first_visible(self) +
                                  static_cast<double>(index - 1));
    } else if (member == "daybold") {
        const double day =
            arguments.empty() ? 0.0 : month_argument(arguments[0].value);
        result = Value{month_state(self).bold.contains(static_cast<long>(day))};
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
        if (month_truthy(value)) {
            month_state(self).bold.insert(day);
        } else {
            month_state(self).bold.erase(day);
        }
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
        const double wanted = std::floor(date->serial);
        if (member == "mindate" || member == "maxdate") {
            const double low = member == "mindate" ? wanted : month_min(self);
            const double high = member == "maxdate" ? wanted : month_max(self);
            if (low > high) {
                return invalid();
            }
            self.values[member] = month_date_value(wanted);
            const double current = month_value(self);
            if (current < low) {
                month_select_one(self, low);
            } else if (current > high) {
                month_select_one(self, high);
            }
            return true;
        }
        if (!month_in_range(self, wanted)) {
            return invalid();
        }
        if (member == "value" || !property_flag(self, "multiselect")) {
            month_select_one(self, wanted);
            return true;
        }
        const long most = std::max(1L, property_whole(self, "maxselcount"));
        double start = member == "selstart" ? wanted : month_sel_start(self);
        double end = member == "selend" ? wanted : month_sel_end(self);
        if (member == "selstart" &&
            (end < start || end - start + 1.0 > static_cast<double>(most))) {
            end = start;  // a new start leaves a one-day selection
        } else if (member == "selend" && end < start) {
            start = end;
        }
        if (end - start + 1.0 > static_cast<double>(most)) {
            return invalid();
        }
        self.values["selstart"] = month_date_value(start);
        self.values["selend"] = month_date_value(end);
        self.values["value"] = month_date_value(start);
        return true;
    }
    if (member == "day" || member == "month" || member == "year") {
        Value converted = value;
        if (!coerce_row(in, self, member, converted, failure)) {
            return true;
        }
        const long number = static_cast<long>(as_double(converted));
        const auto parts = split_date(month_value(self));
        long year = static_cast<long>(parts.year);
        long month = static_cast<long>(parts.month);
        long day = static_cast<long>(parts.day);
        if (member == "year") {
            year = number;
        } else if (member == "month") {
            month = number;
        } else {
            day = number;
        }
        if (year < 1601 || year > 9999 || month < 1 || month > 12 || day < 1 ||
            (member == "day" && day > days_in_month(year, month))) {
            return invalid();
        }
        day = std::min<long>(day, static_cast<long>(days_in_month(year, month)));
        const double serial = date_serial(year, month, day);
        if (!month_in_range(self, serial)) {
            return invalid();
        }
        month_select_one(self, serial);
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
    if (!month_in_range(self, start) || !month_in_range(self, end)) {
        return true;
    }
    if (!property_flag(self, "multiselect")) {
        end = start;
    }
    if (end < start) {
        std::swap(start, end);
    }
    const long most = std::max(1L, property_whole(self, "maxselcount"));
    if (end - start + 1.0 > static_cast<double>(most)) {
        return true;
    }
    const bool changed =
        start != month_sel_start(self) || end != month_sel_end(self);
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

}  // namespace

// Scripted input: `click|dblclick yyyy-mm-dd`, `select yyyy-mm-dd yyyy-mm-dd`.
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
    if (action == "click" || action == "dblclick") {
        return month_user_select(in, self, start, start, action == "dblclick");
    }
    if (action == "select" && month_parse(second, end)) {
        return month_user_select(in, self, start, end, false);
    }
    return true;
}

// What the host draws: the dates, the range and how the calendar is shown.
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
}

// ----- DTPicker ------------------------------------------------------------------
//
// A date and time picker. Value is a Date (a Null Variant while CheckBox is
// True and the box is cleared); Format (0 long date, 1 short date, 2 time, 3
// CustomFormat) says how it is shown; UpDown replaces the drop-down calendar
// with spin arrows. Day, Month, Year, Hour, Minute, Second and DayOfWeek are the
// parts of Value (Null when it is Null). MinDate and MaxDate keep it in range
// (380 outside). Change is raised when Value changes, by code or by the user;
// DropDown and CloseUp bracket the calendar. In scripts: `ui <control> value
// yyyy-mm-dd[ hh:mm:ss]`, `ui <control> check on|off`, `ui <control> dropdown`
// and `ui <control> closeup`. The behaviour follows the documented control;
// there is no reference probe.

namespace {

// Now, as a date serial with the time of day.
[[nodiscard]] double picker_now() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    return date_serial(local.tm_year + 1900, local.tm_mon + 1, local.tm_mday) +
           (local.tm_hour * 3600.0 + local.tm_min * 60.0 + local.tm_sec) /
               86400.0;
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

[[nodiscard]] bool picker_in_range(ControlObject& self, const double serial) {
    const double day = std::floor(serial);
    return day >= month_date_of(self, "mindate", month_first_date()) &&
           day <= month_date_of(self, "maxdate", month_last_date());
}

// `yyyy-mm-dd` with an optional ` hh:mm:ss` to a serial.
[[nodiscard]] bool picker_parse(const std::string& date, const std::string& time,
                                double& serial) {
    double day = 0;
    if (!month_parse(date, day)) {
        return false;
    }
    double fraction = 0;
    if (!time.empty()) {
        char* end = nullptr;
        const long hour = std::strtol(time.c_str(), &end, 10);
        const long minute = *end == ':' ? std::strtol(end + 1, &end, 10) : 0;
        const long second = *end == ':' ? std::strtol(end + 1, &end, 10) : 0;
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 ||
            second > 59) {
            return false;
        }
        fraction = (hour * 3600.0 + minute * 60.0 + second) / 86400.0;
    }
    serial = day + fraction;
    return true;
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
        result = Value{static_cast<Int16>(picker_format(self))};
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
        const auto parts = split_date(picker_value(self));
        const std::int64_t number = member == "day"      ? parts.day
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
        if (std::holds_alternative<Null>(value)) {
            if (!property_flag(self, "checkbox")) {
                return invalid();
            }
            const bool changed = !picker_is_null(self);
            self.values["value"] = Value{Null{}};
            return !changed || raise_event(in, self, "Change", {});
        }
        Value converted = value;
        if (!coerce_property(in, converted, "Date", failure)) {
            return true;
        }
        const auto* date = std::get_if<DateValue>(&converted);
        if (date == nullptr || !picker_in_range(self, date->serial)) {
            return invalid();
        }
        const bool changed =
            picker_is_null(self) || date->serial != picker_value(self);
        self.values["value"] = month_date_value(date->serial);
        return !changed || raise_event(in, self, "Change", {});
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
        const double wanted = std::floor(date->serial);
        const double low =
            member == "mindate" ? wanted
                                : month_date_of(self, "mindate", month_first_date());
        const double high =
            member == "maxdate" ? wanted
                                : month_date_of(self, "maxdate", month_last_date());
        if (low > high) {
            return invalid();
        }
        self.values[member] = month_date_value(wanted);
        if (!picker_is_null(self)) {
            const double current = picker_value(self);
            const double day = std::floor(current);
            const double fraction = current - day;
            if (day < low) {
                self.values["value"] = month_date_value(low + fraction);
            } else if (day > high) {
                self.values["value"] = month_date_value(high + fraction);
            }
        }
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
        const long number = static_cast<long>(as_double(converted));
        const auto parts = split_date(picker_value(self));
        long year = static_cast<long>(parts.year);
        long month = static_cast<long>(parts.month);
        long day = static_cast<long>(parts.day);
        long hour = static_cast<long>(parts.hour);
        long minute = static_cast<long>(parts.minute);
        long second = static_cast<long>(parts.second);
        (member == "year"     ? year
         : member == "month"  ? month
         : member == "day"    ? day
         : member == "hour"   ? hour
         : member == "minute" ? minute
                              : second) = number;
        if (year < 1601 || year > 9999 || month < 1 || month > 12 || day < 1 ||
            hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 ||
            second > 59 ||
            (member == "day" && day > days_in_month(year, month))) {
            return invalid();
        }
        day = std::min<long>(day, static_cast<long>(days_in_month(year, month)));
        const double serial =
            date_serial(year, month, day) +
            (hour * 3600.0 + minute * 60.0 + second) / 86400.0;
        if (!picker_in_range(self, serial)) {
            return invalid();
        }
        const bool changed = serial != picker_value(self);
        self.values["value"] = month_date_value(serial);
        return !changed || raise_event(in, self, "Change", {});
    }
    return false;
}

// Scripted input: the user picks a value, ticks the box, opens the calendar.
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
    if (action == "closeup") {
        return raise_event(in, self, "CloseUp", {});
    }
    if (action == "check") {
        if (!property_flag(self, "checkbox")) {
            return true;
        }
        const bool on = first == "on" || first == "1" || first == "true";
        if (on == !picker_is_null(self)) {
            return true;
        }
        if (on) {
            const auto last = self.values.find("lastvalue");
            self.values["value"] =
                last != self.values.end() ? last->second
                                          : month_date_value(picker_design_date(self));
        } else {
            self.values["lastvalue"] = month_date_value(picker_value(self));
            self.values["value"] = Value{Null{}};
        }
        return raise_event(in, self, "Change", {});
    }
    if (action == "value") {
        double serial = 0;
        if (!picker_parse(first, second, serial) ||
            !picker_in_range(self, serial)) {
            return true;
        }
        const bool changed =
            picker_is_null(self) || serial != picker_value(self);
        self.values["value"] = month_date_value(serial);
        return !changed || raise_event(in, self, "Change", {});
    }
    return true;
}

// What the host draws.
void dtpicker_snapshot(ControlObject& self, ui::CommonSnapshot& snapshot) {
    snapshot.date_value = picker_is_null(self) ? 0.0 : picker_value(self);
    snapshot.date_min = month_date_of(self, "mindate", month_first_date());
    snapshot.date_max = month_date_of(self, "maxdate", month_last_date());
    snapshot.style = picker_format(self);
    snapshot.text = property_text(self, "customformat");
    snapshot.checkboxes = property_flag(self, "checkbox");
    snapshot.select_range = picker_is_null(self);  // the box is cleared
}
