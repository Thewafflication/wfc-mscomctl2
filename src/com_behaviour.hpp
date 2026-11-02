// The behaviour of the controls of the library when a COM client creates them
// (see wfc/ocx/class_factory.hpp). The rules themselves are the library's
// models, which the WFC evaluator uses too. Windows only; internal to the WFC
// libraries.

#ifndef WFC_MSCOMCTL2_COM_BEHAVIOUR_HPP
#define WFC_MSCOMCTL2_COM_BEHAVIOUR_HPP

#include <wfc/mscomctl2/updown.hpp>
#include <wfc/ocx/class_factory.hpp>

#include <memory>
#include <string>

namespace wfc::mscomctl2 {

// An UpDown keeps Min, Max, Value, Increment and Wrap in its model: Value is
// kept inside the range and a value outside it is error 380. The other
// properties are stored by the control as for any member.
class UpDownBehaviour final : public wfc::com::Behaviour {
public:
    HRESULT put(const std::string& member, const VARIANT& value,
                long* error) override {
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
        return S_OK;
    }

    HRESULT get(const std::string& member, VARIANT* result) override {
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

// The behaviour of the class called `class_name`, none for most of them yet.
inline std::unique_ptr<wfc::com::Behaviour> make_behaviour(
    const char* class_name) {
    if (_stricmp(class_name, "UpDown") == 0) {
        return std::make_unique<UpDownBehaviour>();
    }
    return nullptr;
}

}  // namespace wfc::mscomctl2

#endif  // WFC_MSCOMCTL2_COM_BEHAVIOUR_HPP
