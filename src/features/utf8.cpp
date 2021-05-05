#include "../test.h"
#include "mfbt/Result.h"
#include "mfbt/ResultVariant.h"
#include "mfbt/Span.h"
#include "mfbt/UniquePtr.h"
#include "mfbt/Vector.h"
#include <string_view>

namespace features::utf8 {
using namespace mozilla;

/**
 * A custom string-handling class with UTF-8 support.
 */
class UTF8 final {
public:
  // Move only
  UTF8(UTF8 &&other) = default;
  UTF8 &operator=(UTF8 &&other) = default;

  enum class Error { InvalidEncoding };

  static Result<UTF8, Error> TryCreate(const char *aCString) {
    size_t len = strlen(aCString);
    static_assert(sizeof(char) == sizeof(uint8_t),
                  "The UTF8 class expects char to be the same size as uint8_t.");
    return TryCreate(Span{reinterpret_cast<const uint8_t *>(aCString), len});
  }

  static Result<UTF8, Error> TryCreate(Span<const uint8_t> aData) {
    // Validate the ascii

    // | Code Points        | Bytes                                        |
    // |--------------------|----------------------------------------------|
    // |  First    Last     |                                              |
    // |  U+0000   U+007F   | 0xxxxxxx    -           -           -        |
    // |  U+0080   U+07FF   | 110xxxxx    10xxxxxx    -           -        |
    // |  U+0800   U+FFFF   | 1110xxxx    10xxxxxx    10xxxxxx    -        |
    // |  U+10000  U+10FFFF | 11110xxx    10xxxxxx    10xxxxxx    10xxxxxx |

    size_t expectedBytes = 0;
    for (const uint8_t &byte : aData) {
      if (expectedBytes > 0) {
        if (byte >> 6 != 0b10) {
          return Err(Error::InvalidEncoding);
        }
        expectedBytes--;
        continue;
      }
      if (byte >> 7 == 0b0) {
        // Valid ASCII
        continue;
      }
      if (byte >> 5 == 0b110) {
        expectedBytes = 1;
      }
      if (byte >> 4 == 0b1110) {
        expectedBytes = 2;
      }
      if (byte >> 3 == 0b11110) {
        expectedBytes = 3;
      }
    }
    return UTF8(aData);
  }

  bool IsAscii() const {
    for (const uint8_t &byte : mData) {
      if (byte >> 7 != 0b0) {
        return false;
      }
    }
    return true;
  }

  std::string_view GetStringView() const {
    return std::string_view{reinterpret_cast<const char *>(mData.begin()),
                            mData.length()};
  }

private:
  explicit UTF8(Span<const uint8_t> aData) {
    if (!mData.reserve(aData.size())) {
      return;
    };

    for (const auto &datum : aData) {
      mData.infallibleAppend(datum);
    }
  }

  Vector<uint8_t> mData;
};

void run_tests() {
  test::suite("features::utf8", [&]() {
    test::describe("basic ascii example", [&]() {
      uint8_t data[]{72, 69, 76, 76, 79};
      auto string = UTF8::TryCreate(Span{data}).unwrap();
      test::equal(string.GetStringView(), "HELLO");
      test::ok(string.IsAscii(), "The text is ASCII");
    });

    test::describe("from const char*", [&]() {
      auto string = UTF8::TryCreate("This is text.").unwrap();
      test::equal(string.GetStringView(), "This is text.");
      test::ok(string.IsAscii(), "The text is ASCII");
    });

    test::describe("with non-ascii characters", [&]() {
      auto string = UTF8::TryCreate("ᏣᎳᎩ ᎦᏬᏂᎯᏍᏗ").unwrap();
      test::equal(string.GetStringView(), "ᏣᎳᎩ ᎦᏬᏂᎯᏍᏗ");
      test::ok(!string.IsAscii(), "The text is not ASCII");
    });
  });
}

} // namespace features::utf8
