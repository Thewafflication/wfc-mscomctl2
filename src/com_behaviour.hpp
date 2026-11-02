// The behaviour of the controls of the library when a COM client creates them
// (see wfc/ocx/class_factory.hpp). The rules themselves are the library's
// models, which the WFC evaluator uses too. Windows only; internal to the WFC
// libraries.

#ifndef WFC_MSCOMCTL2_COM_BEHAVIOUR_HPP
#define WFC_MSCOMCTL2_COM_BEHAVIOUR_HPP

#include <wfc/mscomctl2/monthview.hpp>
#include <wfc/mscomctl2/updown.hpp>
#include <wfc/ocx/class_factory.hpp>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace wfc::mscomctl2 {

// An UpDown keeps Min, Max, Value, Increment and Wrap in its model: Value is
// kept inside the range and a value outside it is error 380; a change of
// Value raises Change. The other
// properties are stored by the control as for any member.
class UpDownBehaviour final : public wfc::com::Behaviour {
public:
    HRESULT put(const std::string& member, const Index&, const VARIANT& value,
                long* error, std::vector<std::string>* events) override {
        if (member == "wrap") {
            VARIANT flag;
            VariantInit(&flag);
            if (VariantChangeType(&flag, const_cast<VARIANT*>(&value), 0,
                                  VT_BOOL) != S_OK) {
                *error = 13;
                return E_FAIL;
            }
            model_.set_wrap(flag.boolVal != VARIANT_FALSE);
            return S_OK;
        }
        if (member != "min" && member != "max" && member != "value" &&
            member != "increment") {
            return S_FALSE;
        }
        VARIANT number;
        VariantInit(&number);
        if (VariantChangeType(&number, const_cast<VARIANT*>(&value), 0,
                              VT_I4) != S_OK) {
            *error = 13;  // type mismatch
            return E_FAIL;
        }
        const long wanted = number.lVal;
        if (member == "increment") {
            model_.set_increment(wanted);
            return S_OK;
        }
        const auto outcome = member == "min"   ? model_.set_min(wanted)
                             : member == "max" ? model_.set_max(wanted)
                                               : model_.set_value(wanted);
        if (!outcome.valid) {
            *error = 380;  // invalid property value
            return E_FAIL;
        }
        if (outcome.changed) {
            events->push_back("Change");
        }
        return S_OK;
    }

    HRESULT get(const std::string& member, const Index&, VARIANT* result,
                long*) override {
        if (member == "wrap") {
            result->vt = VT_BOOL;
            result->boolVal = model_.wrap() ? VARIANT_TRUE : VARIANT_FALSE;
            return S_OK;
        }
        long number = 0;
        if (member == "min") {
            number = model_.min();
        } else if (member == "max") {
            number = model_.max();
        } else if (member == "value") {
            number = model_.value();
        } else if (member == "increment") {
            number = model_.increment();
        } else {
            return S_FALSE;
        }
        result->vt = VT_I4;
        result->lVal = number;
        return S_OK;
    }

private:
    UpDownModel model_;
};

// A MonthView keeps its dates, range, selection and bold days in its model:
// Value, SelStart, SelEnd, MinDate and MaxDate are dates kept in order and in
// range, Day, Month and Year are the parts of Value, and a date outside the
// range is error 380. DayBold(date) and VisibleDays(index) take an index. The
// other properties are stored by the control as for any member.
class MonthViewBehaviour final : public wfc::com::Behaviour {
public:
    HRESULT put(const std::string& member, const Index& index,
                const VARIANT& value, long* error,
                std::vector<std::string>*) override {
        if (member == "daybold") {
            long date = 0;
            VARIANT flag;
            VariantInit(&flag);
            if (index.count < 1 || !to_date(index.arguments[0], date) ||
                VariantChangeType(&flag, const_cast<VARIANT*>(&value), 0,
                                  VT_BOOL) != S_OK) {
                *error = 13;
                return E_FAIL;
            }
            bold_.set(date, flag.boolVal != VARIANT_FALSE);
            return S_OK;
        }
        if (member == "multiselect") {
            VARIANT flag;
            VariantInit(&flag);
            if (VariantChangeType(&flag, const_cast<VARIANT*>(&value), 0,
                                  VT_BOOL) != S_OK) {
                *error = 13;
                return E_FAIL;
            }
            model_.set_multi_select(flag.boolVal != VARIANT_FALSE);
            return S_OK;
        }
        if (member == "maxselcount" || member == "startofweek" ||
            member == "day" || member == "month" || member == "year") {
            VARIANT number;
            VariantInit(&number);
            if (VariantChangeType(&number, const_cast<VARIANT*>(&value), 0,
                                  VT_I4) != S_OK) {
                *error = 13;
                return E_FAIL;
            }
            if (member == "maxselcount") {
                model_.set_max_sel_count(number.lVal);
                return S_OK;
            }
            if (member == "startofweek") {
                model_.set_first_day(number.lVal);
                return S_OK;
            }
            const auto outcome = model_.set_part(
                member == "year"    ? MonthViewModel::Part::Year
                : member == "month" ? MonthViewModel::Part::Month
                                    : MonthViewModel::Part::Day,
                number.lVal);
            return outcome.valid ? S_OK : refuse(error);
        }
        if (member == "value" || member == "selstart" || member == "selend" ||
            member == "mindate" || member == "maxdate") {
            long date = 0;
            if (!to_date(value, date)) {
                *error = 13;
                return E_FAIL;
            }
            const auto outcome = member == "mindate"   ? model_.set_min(date)
                                 : member == "maxdate" ? model_.set_max(date)
                                 : member == "value"   ? model_.set_value(date)
                                 : member == "selstart"
                                     ? model_.set_sel_start(date)
                                     : model_.set_sel_end(date);
            return outcome.valid ? S_OK : refuse(error);
        }
        return S_FALSE;
    }

    HRESULT get(const std::string& member, const Index& index, VARIANT* result,
                long* error) override {
        if (member == "daybold" || member == "visibledays") {
            long date = 0;
            if (index.count < 1) {
                *error = 13;
                return E_FAIL;
            }
            if (member == "visibledays") {
                VARIANT number;
                VariantInit(&number);
                if (VariantChangeType(&number,
                                      const_cast<VARIANT*>(&index.arguments[0]),
                                      0, VT_I4) != S_OK) {
                    *error = 13;
                    return E_FAIL;
                }
                return model_.visible_day(number.lVal, date)
                           ? give_date(result, date)
                           : refuse(error);
            }
            if (!to_date(index.arguments[0], date)) {
                *error = 13;
                return E_FAIL;
            }
            result->vt = VT_BOOL;
            result->boolVal =
                bold_.is_bold(date) ? VARIANT_TRUE : VARIANT_FALSE;
            return S_OK;
        }
        if (member == "value") {
            return give_date(result, model_.value());
        }
        if (member == "selstart") {
            return give_date(result, model_.sel_start());
        }
        if (member == "selend") {
            return give_date(result, model_.sel_end());
        }
        if (member == "mindate") {
            return give_date(result, model_.min());
        }
        if (member == "maxdate") {
            return give_date(result, model_.max());
        }
        if (member == "multiselect") {
            result->vt = VT_BOOL;
            result->boolVal =
                model_.multi_select() ? VARIANT_TRUE : VARIANT_FALSE;
            return S_OK;
        }
        long number = 0;
        if (member == "day") {
            number = model_.parts().day;
        } else if (member == "month") {
            number = model_.parts().month;
        } else if (member == "year") {
            number = model_.parts().year;
        } else if (member == "dayofweek") {
            number = model_.parts().weekday;
        } else if (member == "week") {
            number = model_.week();
        } else if (member == "maxselcount") {
            number = model_.max_sel_count();
        } else if (member == "startofweek") {
            number = model_.first_day();
        } else {
            return S_FALSE;
        }
        result->vt = VT_I2;
        result->iVal = static_cast<SHORT>(number);
        return S_OK;
    }

private:
    // A Variant as a whole date serial.
    static bool to_date(const VARIANT& value, long& date) {
        VARIANT converted;
        VariantInit(&converted);
        if (VariantChangeType(&converted, const_cast<VARIANT*>(&value), 0,
                              VT_DATE) != S_OK) {
            return false;
        }
        date = static_cast<long>(std::floor(converted.date));
        return true;
    }

    static HRESULT give_date(VARIANT* result, const long date) {
        result->vt = VT_DATE;
        result->date = static_cast<DATE>(date);
        return S_OK;
    }

    static HRESULT refuse(long* error) {
        *error = 380;  // invalid property value
        return E_FAIL;
    }

    MonthViewModel model_{calendar::today()};
    DayBold bold_;
};

// The behaviour of the class called `class_name`, none for most of them yet.
inline std::unique_ptr<wfc::com::Behaviour> make_behaviour(
    const char* class_name) {
    if (_stricmp(class_name, "UpDown") == 0) {
        return std::make_unique<UpDownBehaviour>();
    }
    if (_stricmp(class_name, "MonthView") == 0) {
        return std::make_unique<MonthViewBehaviour>();
    }
    return nullptr;
}

}  // namespace wfc::mscomctl2

#endif  // WFC_MSCOMCTL2_COM_BEHAVIOUR_HPP
