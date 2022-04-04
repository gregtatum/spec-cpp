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

std::string test_pattern(UnicodeString pattern) {
  Locale locale("en");
  UErrorCode status = U_ZERO_ERROR;

  SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
  GregorianCalendar calendar(2020, 0, 21, 14, 5, status);

  UnicodeString formatted;
  formatter->format(calendar.getTime(status), formatted, status);
  return toString(formatted);
}

class SkeletonTestingUtility {
public:
  SkeletonTestingUtility(Locale aLocale, int32_t year, int32_t month, int32_t date,
                         int32_t hour, int32_t minute, UErrorCode status = U_ZERO_ERROR)
      : locale(aLocale),
        calendar(GregorianCalendar(year, month, date, hour, minute, status)) {
    if (U_FAILURE(status)) {
      test::ok(false, "Unable to create gregorian calendar");
    }
    generator = DateTimePatternGenerator::createInstance(locale, status);

    if (U_FAILURE(status)) {
      test::ok(false, "Unable to generate the DateTimePatternGenerator");
    }
  }

  std::string Pattern(UnicodeString skeleton) {
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString pattern = generator->getBestPattern(skeleton, status);

    std::string patternStr;
    pattern.toUTF8String(patternStr);

    return patternStr;
  }

  std::string Format(UnicodeString skeleton) {
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString pattern = generator->getBestPattern(skeleton, status);
    SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
    UnicodeString formatted;
    formatted = formatter->format(calendar.getTime(status), formatted, status);
    return toString(formatted);
  }

  DateTimePatternGenerator *generator;
  Locale locale;
  GregorianCalendar calendar;
};

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

    test::describe("garbage skeletons Latin1", []() {
      Locale locale("en");
      UErrorCode status = U_ZERO_ERROR;
      DateTimePatternGenerator *generator =
          DateTimePatternGenerator::createInstance(locale, status);
      if (U_FAILURE(status)) {
        test::ok(false, "Unable to generate the DateTimePatternGenerator");
        return;
      }

      UnicodeString pattern = generator->getBestPattern(
          UnicodeString("E, M/d/y ('week': w), h a ('second': s)"), status);

      std::string patternStr;
      pattern.toUTF8String(patternStr);

      test::equal(toString(pattern),
                  std::string("EEE, M/d/y ('week': w), h a ('second': s)"),
                  "A pattern can be generated.");

      SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
      GregorianCalendar calendar(2020, 0, 20, 14, 5, status);

      UnicodeString formatted;
      formatted = formatter->format(calendar.getTime(status), formatted, status);

      test::equal(toString(formatted),
                  std::string("Mon, 1/20/2020 (week: 4), 2 PM (second: 0)"),
                  "The date can be formatted");
    });

    test::describe("garbage skeletons beyond the bmp", []() {
      Locale locale("en");
      UErrorCode status = U_ZERO_ERROR;
      DateTimePatternGenerator *generator =
          DateTimePatternGenerator::createInstance(locale, status);
      if (U_FAILURE(status)) {
        test::ok(false, "Unable to generate the DateTimePatternGenerator");
        return;
      }

      UnicodeString pattern =
          generator->getBestPattern(UnicodeString("👍 this is we◌ᷣird"), status);

      std::string patternStr;
      pattern.toUTF8String(patternStr);

      test::equal(toString(pattern), std::string(" ('day': d E), h a ('second': s)"),
                  "A pattern can be generated.");

      SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
      GregorianCalendar calendar(2020, 0, 20, 14, 5, status);

      UnicodeString formatted;
      formatted = formatter->format(calendar.getTime(status), formatted, status);

      test::equal(toString(formatted), std::string(" (day: 20 Mon), 2 PM (second: 0)"),
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

    test::describe("all CLDR skeleton patterns in english", []() {
      // clang-format off
      test::equal(test_pattern("y"),           std::string("2020"));               // skeleton: "y"
      test::equal(test_pattern("M/y"),         std::string("1/2020"));             // skeleton: "yM"
      test::equal(test_pattern("M/d/y"),       std::string("1/21/2020"));          // skeleton: "yMd"
      test::equal(test_pattern("E, M/d/y"),    std::string("Tue, 1/21/2020"));     // skeleton: "yMdE"
      test::equal(test_pattern("MMM y"),       std::string("Jan 2020"));           // skeleton: "yMMM"
      test::equal(test_pattern("MMM d, y"),    std::string("Jan 21, 2020"));       // skeleton: "yMMMd"
      test::equal(test_pattern("E, MMM d, y"), std::string("Tue, Jan 21, 2020"));  // skeleton: "yMMMdE"
      test::equal(test_pattern("MMMM y"),      std::string("January 2020"));       // skeleton: "yMMMM"
      test::equal(test_pattern("L"),           std::string("1"));                  // skeleton: "M"
      test::equal(test_pattern("M/d"),         std::string("1/21"));               // skeleton: "Md"
      test::equal(test_pattern("E, M/d"),      std::string("Tue, 1/21"));          // skeleton: "MdE"
      test::equal(test_pattern("LLL"),         std::string("Jan"));                // skeleton: "MMM"
      test::equal(test_pattern("MMM d"),       std::string("Jan 21"));             // skeleton: "MMMd"
      test::equal(test_pattern("E, MMM d"),    std::string("Tue, Jan 21"));        // skeleton: "MMMdE"
      test::equal(test_pattern("MMMM d"),      std::string("January 21"));         // skeleton: "MMMMd"
      test::equal(test_pattern("d"),           std::string("21"));                 // skeleton: "d"
      test::equal(test_pattern("d E"),         std::string("21 Tue"));             // skeleton: "dE"

      test::equal(test_pattern("ccc"),         std::string("Tue"));                // skeleton: "E"

      test::equal(test_pattern("E h:mm a"),    std::string("Tue 2:05 PM"));        // skeleton: "Ehm"
      test::equal(test_pattern("E h:mm:ss a"), std::string("Tue 2:05:00 PM"));     // skeleton: "Ehms"
      test::equal(test_pattern("E HH:mm"),     std::string("Tue 14:05"));          // skeleton: "EHm"
      test::equal(test_pattern("E HH:mm:ss"),  std::string("Tue 14:05:00"));       // skeleton: "EHms"

      test::equal(test_pattern("h a"),         std::string("2 PM"));               // skeleton: "h"
      test::equal(test_pattern("h:mm a"),      std::string("2:05 PM"));            // skeleton: "hm"
      test::equal(test_pattern("h:mm:ss a"),   std::string("2:05:00 PM"));         // skeleton: "hms"
      test::equal(test_pattern("h:mm:ss a v"), std::string("2:05:00 PM CT"));      // skeleton: "hmsv"
      test::equal(test_pattern("h:mm a v"),    std::string("2:05 PM CT"));         // skeleton: "hmv"
      test::equal(test_pattern("HH"),          std::string("14"));                 // skeleton: "H"
      test::equal(test_pattern("HH:mm"),       std::string("14:05"));              // skeleton: "Hm"
      test::equal(test_pattern("HH:mm:ss"),    std::string("14:05:00"));           // skeleton: "Hms"
      test::equal(test_pattern("HH:mm:ss v"),  std::string("14:05:00 CT"));        // skeleton: "Hmsv"
      test::equal(test_pattern("HH:mm v"),     std::string("14:05 CT"));           // skeleton: "Hmv"
      test::equal(test_pattern("mm:ss"),       std::string("05:00"));              // skeleton: "ms"
      // clang-format on
    });

    test::describe("generate patterns from single input skeletons", []() {
      SkeletonTestingUtility skeletons(Locale("en"), 2020, 0, 20, 14, 5);
      test::equal(skeletons.Pattern("m"), std::string("m"), "Pattern for m");
      test::equal(skeletons.Format("m"), std::string("5"), "Format for m");

      // Era
      test::equal(skeletons.Pattern("G"), std::string("G"), "Pattern for G");
      test::equal(skeletons.Format("G"), std::string("AD"), "Format for G");

      // Calendar year
      test::equal(skeletons.Pattern("y"), std::string("y"), "Pattern for y");
      test::equal(skeletons.Format("y"), std::string("2020"), "Format for y");

      // Year in “Week of Year”
      test::equal(skeletons.Pattern("Y"), std::string("Y"), "Pattern for Y");
      test::equal(skeletons.Format("Y"), std::string("2020"), "Format for Y");

      // Extended year (numeric)
      test::equal(skeletons.Pattern("u"), std::string("y"), "Pattern for u");
      test::equal(skeletons.Format("u"), std::string("2020"), "Format for u");

      // Quarter number/name
      test::equal(skeletons.Pattern("Q"), std::string("Q"), "Pattern for Q");
      test::equal(skeletons.Format("Q"), std::string("1"), "Format for Q");

      // Stand-Alone Quarter
      test::equal(skeletons.Pattern("q"), std::string("q"), "Pattern for q");
      test::equal(skeletons.Format("q"), std::string("1"), "Format for q");

      // Format style month
      test::equal(skeletons.Pattern("M"), std::string("L"), "Pattern for M");
      test::equal(skeletons.Format("M"), std::string("1"), "Format for M");

      // Stand-alone style month
      test::equal(skeletons.Pattern("L"), std::string("L"), "Pattern for L");
      test::equal(skeletons.Format("L"), std::string("1"), "Format for L");

      // Week of year (numeric)
      test::equal(skeletons.Pattern("w"), std::string("w"), "Pattern for w");
      test::equal(skeletons.Format("w"), std::string("4"), "Format for w");

      // Week of Month (numeric)
      test::equal(skeletons.Pattern("W"), std::string("W"), "Pattern for W");
      test::equal(skeletons.Format("W"), std::string("4"), "Format for W");

      // Day of month
      test::equal(skeletons.Pattern("d"), std::string("d"), "Pattern for d");
      test::equal(skeletons.Format("d"), std::string("20"), "Format for d");

      // Day of year
      test::equal(skeletons.Pattern("D"), std::string("D"), "Pattern for D");
      test::equal(skeletons.Format("D"), std::string("20"), "Format for D");

      // Day of week in Month
      test::equal(skeletons.Pattern("F"), std::string("F"), "Pattern for F");
      test::equal(skeletons.Format("F"), std::string("3"), "Format for F");

      // Day of week name, format style
      // "E" => "ccc" in the CLDR
      test::equal(skeletons.Pattern("E"), std::string("ccc"), "Pattern for E");
      test::equal(skeletons.Format("E"), std::string("Mon"), "Format for E");

      // Local day of week number/name, format style.
      // Seems to be following "E" => "ccc" in the CLDR
      test::equal(skeletons.Pattern("e"), std::string("ccc"), "Pattern for e");
      test::equal(skeletons.Format("e"), std::string("Mon"), "Format for e");

      // Stand-alone local day
      // Seems to be following "E" => "ccc" in the CLDR
      test::equal(skeletons.Pattern("c"), std::string("ccc"), "Pattern for c");
      test::equal(skeletons.Format("c"), std::string("Mon"), "Format for c");

      // am / pm
      test::equal(skeletons.Pattern("a"), std::string("a"), "Pattern for a");
      test::equal(skeletons.Format("a"), std::string("PM"), "Format for a");

      // am, pm, noon, midnight
      test::equal(skeletons.Pattern("b"), std::string("b"), "Pattern for b");
      test::equal(skeletons.Format("b"), std::string("PM"), "Format for b");

      // Flexible day periods
      test::equal(skeletons.Pattern("B"), std::string("B"), "Pattern for B");
      test::equal(skeletons.Format("B"), std::string("in the afternoon"), "Format for B");

      // Hour 1-12
      // Matches skeleton in the CLDR: "h" => "h a"
      test::equal(skeletons.Pattern("h"), std::string("h a"), "Pattern for h");
      test::equal(skeletons.Format("h"), std::string("2 PM"), "Format for h");

      // Matches the skeleton "H" => "HH"
      test::equal(skeletons.Pattern("H"), std::string("HH"), "Pattern for H");
      test::equal(skeletons.Format("H"), std::string("14"), "Format for H");

      // Hour [0-11]. When used in a skeleton, only matches K or h, see above.
      test::equal(skeletons.Pattern("K"), std::string("h a"), "Pattern for K");
      test::equal(skeletons.Format("K"), std::string("2 PM"), "Format for K");

      // Hour [1-24]. When used in a skeleton, only matches K or h, see above.
      test::equal(skeletons.Pattern("k"), std::string("HH"), "Pattern for k");
      test::equal(skeletons.Format("k"), std::string("14"), "Format for k");

      // Minute (numeric)
      test::equal(skeletons.Pattern("m"), std::string("m"), "Pattern for m");
      test::equal(skeletons.Format("m"), std::string("5"), "Format for m");

      // Second (numeric)
      test::equal(skeletons.Pattern("s"), std::string("s"), "Pattern for s");
      test::equal(skeletons.Format("s"), std::string("0"), "Format for s");

      // Fractional second
      test::equal(skeletons.Pattern("S"), std::string("S"), "Pattern for S");
      test::equal(skeletons.Format("S"), std::string("0"), "Format for S");

      // Milliseconds in day
      test::equal(skeletons.Pattern("A"), std::string("A"), "Pattern for A");
      test::equal(skeletons.Format("A"), std::string("50700000"), "Format for A");
    });

    test::describe("Locale canonicalization", []() {
      test::equal(Locale::createCanonical("en-US").getBaseName(), "en_US",
                  "Canonicalizes en-US");
      test::equal(Locale::createCanonical("en-US-posix").getBaseName(), "en_US_POSIX",
                  "Canonicalizes POSIX");
      test::equal(Locale::createCanonical("und").getBaseName(), "",
                  "und gets changed to an empty string");
      test::equal(Locale::createCanonical("asdf").getBaseName(), "asdf",
                  "retains incorrect locales");
      test::equal(Locale::createCanonical("es-es").getBaseName(), "es_ES",
                  "makes text uppercase");
      test::equal(Locale::createCanonical("👍").getBaseName(), "👍",
                  "Accepts non-ascii inputs");
      test::equal(Locale::createCanonical("en-USA").getBaseName(), "en_US",
                  "Converts 3 letter country codes to 2 letter.");
      test::equal(Locale::createCanonical("fr-fr@EURO").getBaseName(), "fr_FR_EURO",
                  "Does not perform level 2 canonicalization where the result would be "
                  "fr_FR@currency=EUR");
      test::equal(Locale::createCanonical("ar-MA.utf8").getBaseName(), "ar_MA",
                  "Removes the .utf8 ends");
    });
  });
}

} // namespace icu
} // namespace features
