#include "../test.h"
#include "unicode/calendar.h"
#include "unicode/dtptngen.h"
#include "unicode/gregocal.h"
#include "unicode/numberformatter.h"
#include "unicode/smpdtfmt.h"
#include "unicode/udat.h"
#include "unicode/utypes.h"
#include <codecvt>

namespace features {
namespace icu_capi {

void icu_tests() {}

std::string toString(UChar *u16) {
  return std::wstring_convert<std::codecvt_utf8_utf16<UChar>, UChar>{}.to_bytes(
      u16);
}

void run_tests() {
  test::suite("features::icu_capi", []() {
    test::describe("udat_format with the C api", []() {
      // Set up the variable bindings.
      UErrorCode status = U_ZERO_ERROR;
      UChar *formattedString; // UChar* is a pointer to a null-terminated UTF-16
                              // string.
      int32_t stringLength = 200;
      double unixEpoch = 1618431049.0;

      // Create a formatter.
      UDateFormat *dateFormat =
          udat_open(UDAT_SHORT, UDAT_NONE, "en", NULL, -1, NULL, -1, &status);

      if (U_FAILURE(status)) {
        test::ok(false, "Unable to open a UDateFormat.");
        return;
      }

      // Oh joyous of days.
      formattedString = (UChar *)malloc(sizeof(UChar) * (stringLength + 1));
      test::ok(formattedString, "Malloc succeeded.");

      udat_format(dateFormat, unixEpoch, formattedString, stringLength + 1,
                  NULL, &status);

      if (U_FAILURE(status)) {
        test::ok(false, "Unable to run udat_format.");
        return;
      }

      test::equal(toString(formattedString), std::string("11:33 AM"),
                  "The strings are equal");

      free(formattedString);
      udat_close(dateFormat);
    });

    test::describe("udat_format with the C api, truncated", []() {
      // Set up the variable bindings.
      UErrorCode status = U_ZERO_ERROR;
      double unixEpoch = 1618431049.0;

      // Create a formatter.
      UDateFormat *dateFormat =
          udat_open(UDAT_SHORT, UDAT_NONE, "en", NULL, -1, NULL, -1, &status);

      test::equal(status, U_ZERO_ERROR, "Able to create a date format object.");

      // Get the size of the string's buffer.
      int32_t size =
          udat_format(dateFormat, unixEpoch, nullptr, 0, NULL, &status);

      size++; // Plus the null terminator character.
      test::equal(status, U_BUFFER_OVERFLOW_ERROR, "The buffer overflowed.");
      status = U_ZERO_ERROR;

      // Write the formatted string into the buffer.
      std::basic_string<char16_t> formattedString;
      formattedString.resize(size);
      udat_format(dateFormat, unixEpoch, formattedString.data(), size, NULL,
                  &status);

      test::equal(status, U_ZERO_ERROR,
                  "Formatted the string correctly into the buffer.");

      test::equal(toString(formattedString.data()), std::string("11:33 AM"),
                  "The strings are equal");

      udat_close(dateFormat);
    });
  });
}

} // namespace icu_capi
} // namespace features
