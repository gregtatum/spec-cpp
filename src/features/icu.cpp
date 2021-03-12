#include "../test.h"
#include "unicode/calendar.h"
#include "unicode/dtptngen.h"
#include "unicode/gregocal.h"
#include "unicode/numberformatter.h"
#include "unicode/smpdtfmt.h"
#include "unicode/unum.h"
#include "unicode/utypes.h"

namespace features {
namespace icu {

using ::icu::Calendar;
using ::icu::CurrencyUnit;
using ::icu::DateTimePatternGenerator;
using ::icu::GregorianCalendar;
using ::icu::Locale;
using ::icu::SimpleDateFormat;
using ::icu::UnicodeString;
using ::icu::number::Notation;
using ::icu::number::NumberFormatter;
using ::icu::number::Precision;

std::string toString(UnicodeString &unicodeString) {
  std::string stdString;
  unicodeString.toUTF8String(stdString);
  return stdString;
}

void icu_tests() {}

void run_tests() {
  test::suite("features::icu", []() {
    test::describe("basic usage", []() {
      Locale locale("en");
      UErrorCode status = U_ZERO_ERROR;
      DateTimePatternGenerator *generator =
          DateTimePatternGenerator::createInstance(locale, status);
      if (U_FAILURE(status)) {
        test::ok(false, "Unable to generate the DateTimePatternGenerator");
        return;
      }

      UnicodeString pattern =
          generator->getBestPattern(UnicodeString("MMMd"), status);

      std::string patternStr;
      pattern.toUTF8String(patternStr);

      test::equal(toString(pattern), std::string("MMM d"),
                  "A pattern can be generated.");

      SimpleDateFormat *formatter =
          new SimpleDateFormat(pattern, locale, status);
      GregorianCalendar calendar(status);
      calendar.set(2020, 0, 20);

      UnicodeString formatted;
      formatted =
          formatter->format(calendar.getTime(status), formatted, status);

      test::equal(toString(formatted), std::string("Jan 20"),
                  "The date can be formatted");
    });

    test::describe("number format", []() {
      UErrorCode statusCurrency = U_ZERO_ERROR;
      UErrorCode statusFormatInt = U_ZERO_ERROR;
      UErrorCode statusToString = U_ZERO_ERROR;

      auto result = NumberFormatter::withLocale(Locale("en"))
                        .notation(Notation::compactShort())
                        .unit(CurrencyUnit("USD", statusCurrency))
                        .precision(Precision::maxFraction(2))
                        .formatInt(1234, statusFormatInt)
                        .toString(statusToString);

      test::ok(!U_FAILURE(statusCurrency), "CurrencyUnit had no errors.");
      test::ok(!U_FAILURE(statusFormatInt), "FormatInt had no errors.");
      test::ok(!U_FAILURE(statusToString), "ToString had no errors.");

      test::equal(toString(result), std::string("$1.23K"),
                  "The number can be formatted.");
    });

    test::describe("fixed decimal format approximation", []() {
      UErrorCode status = U_ZERO_ERROR;

      auto result = NumberFormatter::withLocale(Locale("en"))
                        .notation(Notation::simple())
                        .precision(Precision::maxFraction(2))
                        .formatInt(1234, status)
                        .toString(status);

      test::ok(!U_FAILURE(status), "Formatting had no errors.");
      test::equal(toString(result), std::string("1,234"),
                  "The number can be formatted.");
    });

    test::describe("fixed decimal format approximation", []() {
      UErrorCode status = U_ZERO_ERROR;

      auto result = NumberFormatter::withLocale(Locale("en"))
                        .notation(Notation::simple())
                        .precision(Precision::maxFraction(2))
                        .formatDouble(1234.56789, status)
                        .toString(status);

      test::ok(!U_FAILURE(status), "Formatting had no errors.");
      test::equal(toString(result), std::string("1,234.57"),
                  "The number can be formatted.");
    });

    test::describe("fixed decimal format approximation", []() {
      UErrorCode status = U_ZERO_ERROR;

      auto result = NumberFormatter::withLocale(Locale("en"))
                        .notation(Notation::simple())
                        .precision(Precision::maxFraction(0))
                        .formatDouble(1234.56789, status)
                        .toString(status);

      test::ok(!U_FAILURE(status), "Formatting had no errors.");
      test::equal(toString(result), std::string("1,235"),
                  "The number can be formatted.");
    });
  });
}

} // namespace icu
} // namespace features
