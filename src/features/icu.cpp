#include "../test.h"
#include "unicode/calendar.h"
#include "unicode/dtptngen.h"
#include "unicode/gregocal.h"
#include "unicode/smpdtfmt.h"
#include "unicode/utypes.h"

namespace features {
namespace icu {

using ::icu::Calendar;
using ::icu::DateTimePatternGenerator;
using ::icu::GregorianCalendar;
using ::icu::Locale;
using ::icu::SimpleDateFormat;
using ::icu::UnicodeString;

std::string toString(UnicodeString &unicodeString) {
  std::string stdString;
  unicodeString.toUTF8String(stdString);
  return stdString;
}

void icu_tests() {
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

  SimpleDateFormat *formatter = new SimpleDateFormat(pattern, locale, status);
  GregorianCalendar calendar(status);
  calendar.set(2020, 0, 20);

  UnicodeString formatted;
  formatted = formatter->format(calendar.getTime(status), formatted, status);

  test::equal(toString(formatted), std::string("Jan 20"),
              "The date can be formatted");
}

void run_tests() {
  test::suite("features::icu",
              []() { test::describe("basic usage", []() { icu_tests(); }); });
}

} // namespace icu
} // namespace features
