// Calendar arithmetic for the date controls (MonthView, DTPicker): date serials
// (days since 1899-12-30, as Visual Basic counts them), their parts, weeks and
// the `yyyy-mm-dd` text of the test scripts. Header only, with no dependency on
// the WFC evaluator or on Windows. Internal to the WFC libraries.

#ifndef WFC_MSCOMCTL2_CALENDAR_HPP
#define WFC_MSCOMCTL2_CALENDAR_HPP

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

namespace wfc::mscomctl2 {

namespace calendar {

[[nodiscard]] inline long days_from_civil(long year, const long month,
                                          const long day) {
    year -= month <= 2 ? 1 : 0;
    const long era = (year >= 0 ? year : year - 399) / 400;
    const long year_of_era = year - era * 400;
    const long day_of_year =
        (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
    const long day_of_era =
        year_of_era * 365 + year_of_era / 4 - year_of_era / 100 + day_of_year;
    return era * 146097 + day_of_era - 719468;
}

inline void civil_from_days(long days, long& year, long& month, long& day) {
    days += 719468;
    const long era = (days >= 0 ? days : days - 146096) / 146097;
    const long day_of_era = days - era * 146097;
    const long year_of_era = (day_of_era - day_of_era / 1460 +
                              day_of_era / 36524 - day_of_era / 146096) /
                             365;
    year = year_of_era + era * 400;
    const long day_of_year =
        day_of_era - (365 * year_of_era + year_of_era / 4 - year_of_era / 100);
    const long mp = (5 * day_of_year + 2) / 153;
    day = day_of_year - (153 * mp + 2) / 5 + 1;
    month = mp < 10 ? mp + 3 : mp - 9;
    year += month <= 2 ? 1 : 0;
}

// 1970-01-01 as a date serial.
inline constexpr long kEpochOffset = 25569;

[[nodiscard]] inline bool is_leap_year(const long year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

[[nodiscard]] inline long days_in_month(const long year, const long month) {
    constexpr long lengths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month == 2 && is_leap_year(year) ? 29 : lengths[month - 1];
}

// The serial of a year, month (1 to 12) and day.
[[nodiscard]] inline long serial(const long year, const long month,
                                 const long day) {
    return days_from_civil(year, month, day) + kEpochOffset;
}

struct Parts {
    long year = 0;
    long month = 0;
    long day = 0;
    long weekday = 0;  // 1 = Sunday
};

[[nodiscard]] inline Parts parts(const long date) {
    Parts result;
    civil_from_days(date - kEpochOffset, result.year, result.month, result.day);
    result.weekday = ((date % 7) + 7 + 6) % 7 + 1;
    return result;
}

// The first day of week 1 of `year` for weeks starting on `first_day` (1 =
// Sunday), week 1 being the first week of at least four days of the year
// (vbFirstFourDays; measured under VB6, which numbers 7 November 2026 as week
// 44 while the weeks that hold January 1 would make it 45).
[[nodiscard]] inline long first_week_start(const long year,
                                           const long first_day) {
    const long jan1 = serial(year, 1, 1);
    const long offset = (parts(jan1).weekday - first_day + 7) % 7;
    const long start = jan1 - offset;
    return 7 - offset >= 4 ? start : start + 7;
}

// The week of the year for weeks starting on `first_day` (1 = Sunday).
[[nodiscard]] inline long week_of_year(const long date, const long first_day) {
    const long year = parts(date).year;
    if (date >= first_week_start(year + 1, first_day)) {
        return 1;
    }
    const long start = first_week_start(year, first_day);
    if (date < start) {
        return (date - first_week_start(year - 1, first_day)) / 7 + 1;
    }
    return (date - start) / 7 + 1;
}

// Today's date (the local one).
[[nodiscard]] inline long today() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    return serial(local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
}

// The time of day now, as the fraction of a day a Date holds.
[[nodiscard]] inline double time_of_day() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    return (local.tm_hour * 3600.0 + local.tm_min * 60.0 + local.tm_sec) /
           86400.0;
}

inline constexpr int kFirstYear = 1601;
inline constexpr int kLastYear = 9999;

[[nodiscard]] inline long first_date() {
    return serial(kFirstYear, 1, 1);
}

[[nodiscard]] inline long last_date() {
    return serial(kLastYear, 12, 31);
}

// A date and a time of day, from a serial with the time as its fraction.
struct DateTime {
    long year = 0;
    long month = 0;
    long day = 0;
    long hour = 0;
    long minute = 0;
    long second = 0;
    long weekday = 0;  // 1 = Sunday
};

[[nodiscard]] inline DateTime datetime_parts(const double value) {
    double whole = std::floor(value);
    long seconds = std::lround((value - whole) * 86400.0);
    if (seconds >= 86400) {
        seconds -= 86400;
        whole += 1.0;
    }
    const Parts day = parts(static_cast<long>(whole));
    return {day.year,       day.month,           day.day,
            seconds / 3600, seconds % 3600 / 60, seconds % 60,
            day.weekday};
}

// The time of day as the fraction of a day.
[[nodiscard]] inline double time_fraction(const long hour, const long minute,
                                          const long second) {
    return (static_cast<double>(hour) * 3600.0 +
            static_cast<double>(minute) * 60.0 + static_cast<double>(second)) /
           86400.0;
}

// Now (the local time), as a serial with the time of day.
[[nodiscard]] inline double now() {
    const std::time_t clock = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &clock);
#else
    localtime_r(&clock, &local);
#endif
    return static_cast<double>(
               serial(local.tm_year + 1900, local.tm_mon + 1, local.tm_mday)) +
           time_fraction(local.tm_hour, local.tm_min, local.tm_sec);
}

// `yyyy-mm-dd` to a date serial; false for anything else.
[[nodiscard]] inline bool parse_date(const std::string& text, long& date) {
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
    if (*end != '\0' || year < kFirstYear || year > kLastYear || month < 1 ||
        month > 12 || day < 1 || day > days_in_month(year, month)) {
        return false;
    }
    date = serial(year, month, day);
    return true;
}

// `yyyy-mm-dd` with an optional `hh[:mm[:ss]]` to a serial.
[[nodiscard]] inline bool parse_datetime(const std::string& date,
                                         const std::string& time,
                                         double& value) {
    long day = 0;
    if (!parse_date(date, day)) {
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
        fraction = time_fraction(hour, minute, second);
    }
    value = static_cast<double>(day) + fraction;
    return true;
}

}  // namespace calendar

}  // namespace wfc::mscomctl2

#endif  // WFC_MSCOMCTL2_CALENDAR_HPP
