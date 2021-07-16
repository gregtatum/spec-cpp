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

      UnicodeString pattern = generator->getBestPattern(UnicodeString("MMMd"), status);

      std::string patternStr;
      pattern.toUTF8String(patternStr);

      test::equal(toString(pattern), std::string("MMM d"), "A pattern can be generated.");

      SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
      GregorianCalendar calendar(2020, 0, 20, 14, 5, status);

      UnicodeString formatted;
      formatted = formatter->format(calendar.getTime(status), formatted, status);

      test::equal(toString(formatted), std::string("Jan 20"),
                  "The date can be formatted");
    });

    test::describe("time zone testing", []() {
      Locale locale("en");
      UErrorCode status = U_ZERO_ERROR;
      DateTimePatternGenerator *generator =
          DateTimePatternGenerator::createInstance(locale, status);
      if (U_FAILURE(status)) {
        test::ok(false, "Unable to generate the DateTimePatternGenerator");
        return;
      }

      UnicodeString pattern = generator->getBestPattern(UnicodeString("hm zzzz"), status);

      std::string patternStr;
      pattern.toUTF8String(patternStr);

      test::equal(toString(pattern), std::string("h:mm a zzzz"),
                  "A pattern can be generated.");

      SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
      GregorianCalendar calendar(2020, 0, 20, 14, 5, status);

      UnicodeString formatted;
      formatted = formatter->format(calendar.getTime(status), formatted, status);

      test::equal(toString(formatted), std::string("2:05 PM Central Standard Time"),
                  "The date can be formatted");
    });

    test::describe("combine date and time when formatting dates", []() {
      // Apply this field:
      // https://github.com/unicode-org/cldr-json/blob/master/cldr-json/cldr-dates-full/main/en-US-POSIX/ca-gregorian.json#L336
      Locale locale("en");
      UErrorCode status = U_ZERO_ERROR;
      DateTimePatternGenerator *generator =
          DateTimePatternGenerator::createInstance(locale, status);
      if (U_FAILURE(status)) {
        test::ok(false, "Unable to generate the DateTimePatternGenerator");
        return;
      }

      UnicodeString pattern =
          generator->getBestPattern(UnicodeString("yMMMdEhm"), status);

      std::string patternStr;
      pattern.toUTF8String(patternStr);

      test::equal(toString(pattern), std::string("EEE, MMM d, y, h:mm a"),
                  "A pattern can be generated.");

      SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
      GregorianCalendar calendar(2020, 0, 20, 14, 5, status);

      UnicodeString formatted;
      formatted = formatter->format(calendar.getTime(status), formatted, status);

      test::equal(toString(formatted), std::string("Mon, Jan 20, 2020, 2:05 PM"),
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

    test::describe("empty string skeleton", []() {
      Locale locale("en");
      UErrorCode status = U_ZERO_ERROR;
      DateTimePatternGenerator *generator =
          DateTimePatternGenerator::createInstance(locale, status);
      if (U_FAILURE(status)) {
        test::ok(false, "Unable to generate the DateTimePatternGenerator");
        return;
      }

      UnicodeString pattern = generator->getBestPattern(UnicodeString(""), status);

      std::string patternStr;
      pattern.toUTF8String(patternStr);

      test::equal(toString(pattern), std::string(""), "A pattern can be generated.");

      SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
      GregorianCalendar calendar(2020, 0, 20, 14, 5, status);

      UnicodeString formatted;
      formatted = formatter->format(calendar.getTime(status), formatted, status);

      test::equal(toString(formatted), std::string(""), "The date can be formatted");
    });

    test::describe("fixed decimal format approximation", []() {
      UErrorCode status = U_ZERO_ERROR;

      auto result = NumberFormatter::withLocale(Locale("en"))
                        .notation(Notation::simple())
                        .precision(Precision::maxFraction(2))
                        .formatInt(1234, status)
                        .toString(status);

      test::ok(!U_FAILURE(status), "Formatting had no errors.");
      test::equal(toString(result), std::string("1,234"), "The number can be formatted.");
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
      test::equal(toString(result), std::string("1,235"), "The number can be formatted.");
    });

    test::describe("h11 time", []() {
      Locale locale("ja");
      UErrorCode status = U_ZERO_ERROR;
      DateTimePatternGenerator *generator =
          DateTimePatternGenerator::createInstance(locale, status);
      if (U_FAILURE(status)) {
        test::ok(false, "Unable to generate the DateTimePatternGenerator");
        return;
      }

      UnicodeString pattern = generator->getBestPattern(UnicodeString("hm"), status);

      std::string patternStr;
      pattern.toUTF8String(patternStr);

      test::equal(toString(pattern), std::string("aK:mm"),
                  "A pattern can be generated with the K symbol.");

      for (size_t i = 0; i < 24; i++) {
        SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);

        GregorianCalendar calendar(2020, 0, 20, i, 0, status);
        UnicodeString formatted;
        formatted = formatter->format(calendar.getTime(status), formatted, status);
        printf(" - K %zu %s\n", i, toString(formatted).c_str());
      }

      UnicodeString pattern2 = "ah:mm";
      for (size_t i = 0; i < 24; i++) {
        SimpleDateFormat *formatter = new SimpleDateFormat(pattern2, locale, status);

        GregorianCalendar calendar(2020, 0, 20, i, 0, status);
        UnicodeString formatted;
        formatted = formatter->format(calendar.getTime(status), formatted, status);
        printf(" - h %zu %s\n", i, toString(formatted).c_str());
      }

      // test::equal(toString(formatted), std::string("午後2:05"),
      //             "The date can be formatted");
    });
  });
}

} // namespace icu
} // namespace features
