#include "../test.h"
#include "./utf8_codepoints.h"
#include "mfbt/Maybe.h"
#include "mfbt/Result.h"
#include "mfbt/ResultVariant.h"
#include "mfbt/Span.h"
#include "mfbt/UniquePtr.h"
#include "mfbt/Vector.h"
#include <string_view>

namespace features::utf8 {
using namespace mozilla;

const char *btb(int x) {
  auto b = new char[9];
  b[0] = '\0';

  int z;
  for (z = 128; z > 0; z >>= 1) {
    strcat(b, ((x & z) == z) ? "1" : "0");
  }

  return b;
}

const char *btb(uint32_t x) {
  auto b = new char[33];
  b[0] = '\0';

  uint32_t z;
  for (z = 1 << 31; z > 0; z >>= 1) {
    strcat(b, ((x & z) == z) ? "1" : "0");
  }

  return b;
}

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
    if (expectedBytes > 0) {
      // No bytes should be remaining.
      return Err(Error::InvalidEncoding);
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

  class CodePointIter {
  public:
    explicit CodePointIter(UTF8 &aUtf8) : mUtf8(aUtf8) {}

    Maybe<uint32_t> Next() {
      if (mIndex == mUtf8.mData.length()) {
        return Nothing{};
      }
      uint8_t first_byte = mUtf8.mData[mIndex++];
      uint8_t out[4]{0, 0, 0, 0};

      if (first_byte >> 5 == 0b110) {
        uint8_t a = first_byte & 0b00011111;
        uint8_t b = mUtf8.mData[mIndex++] & 0b00111111;

        out[0] = (b & 0b00111111) | (a << 6);
        out[1] = a >> 2;

        // printf("(2) ");
      } else if (first_byte >> 4 == 0b1110) {
        uint8_t a = first_byte & 0b00001111;
        uint8_t b = mUtf8.mData[mIndex++] & 0b00111111;
        uint8_t c = mUtf8.mData[mIndex++] & 0b00111111;

        // printf("a: %s\n", btb(a));
        // printf("b: %s\n", btb(b));
        // printf("c: %s\n", btb(c));

        out[0] = c | (b << 6);
        out[1] = (b >> 2) | a << 4;
        // printf("(3) ");
        // printf("%zu:    %s %s %s %s\n", mIndex - 1,
        //        //
        //        btb(out[0]), btb(out[1]), btb(out[2]), btb(out[3]));

        // printf("utf8:     %s %s %s\n",
        //        //
        //        btb(mUtf8.mData[0]), btb(mUtf8.mData[1]), btb(mUtf8.mData[2]));

      } else if (first_byte >> 3 == 0b11110) {
        uint8_t a = first_byte & 0b00000111;
        uint8_t b = mUtf8.mData[mIndex++] & 0b00111111;
        uint8_t c = mUtf8.mData[mIndex++] & 0b00111111;
        uint8_t d = mUtf8.mData[mIndex++] & 0b00111111;
        // ccdd_dddd
        out[0] = d | (c << 6);
        // bbbb_cccc
        out[1] = (c >> 2) | b << 4;
        // 00aa_aabb
        out[2] = (a << 2) | b >> 4;

        // printf("(4) ");
      } else {
        // printf("(1) ");
        out[0] = first_byte;
      }

      assert(mIndex <= mUtf8.mData.length());

      // printf("%zu:    %s %s %s %s\n", mIndex - 1,
      //        //
      //        btb(out[0]), btb(out[1]), btb(out[2]), btb(out[3]));

      if constexpr (std::endian::native == std::endian::big) {
        return Some((out[0] << 24) | (out[1] << 16) | (out[2] << 8) | out[3]);
      } else {
        return Some((out[3] << 24) | (out[2] << 16) | (out[1] << 8) | out[0]);
      }
    }

  private:
    UTF8 &mUtf8;
    size_t mIndex = 0;
  };

  CodePointIter Iter() { return CodePointIter(*this); }

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

  friend class CodePointIter;
};

void run_tests() {
  test::suite("features::utf8", []() {
    test::describe("basic ascii example", []() {
      uint8_t data[]{72, 69, 76, 76, 79};
      auto string = UTF8::TryCreate(Span{data}).unwrap();
      test::equal(string.GetStringView(), "HELLO");
      test::ok(string.IsAscii(), "The text is ASCII");
    });

    test::describe("from const char*", []() {
      auto string = UTF8::TryCreate("This is text.").unwrap();
      test::equal(string.GetStringView(), "This is text.");
      test::ok(string.IsAscii(), "The text is ASCII");
    });

    test::describe("with non-ascii characters", []() {
      auto string = UTF8::TryCreate("ᏣᎳᎩ ᎦᏬᏂᎯᏍᏗ").unwrap();
      test::equal(string.GetStringView(), "ᏣᎳᎩ ᎦᏬᏂᎯᏍᏗ");
      std::cout << "Here:" << string.GetStringView() << "\n";
      test::ok(!string.IsAscii(), "The text is not ASCII");
    });

    test::describe("iterate on codepoint", []() {
      uint8_t data[]{72, 69, 76, 76, 79};
      auto string = UTF8::TryCreate(Span{data}).unwrap();
      auto iter = string.Iter();
      test::equal(iter.Next(), Some((uint32_t)72));
      test::equal(iter.Next(), Some((uint32_t)69));
      test::equal(iter.Next(), Some((uint32_t)76));
      test::equal(iter.Next(), Some((uint32_t)76));
      test::equal(iter.Next(), Some((uint32_t)79));
      test::ok(!iter.Next(), "Is nothing");
      test::ok(!iter.Next(), "Is nothing");
    });

    test::describe("iterate on codepoint", []() {
      auto string = UTF8::TryCreate("ᏣᎳᎩ").unwrap();
      auto iter = string.Iter();

      test::equal(iter.Next(), Some(C_Ꮳ));
      test::equal(iter.Next(), Some(C_Ꮃ));
      test::equal(iter.Next(), Some(C_Ꭹ));

      test::ok(!iter.Next(), "Is nothing");
      test::ok(!iter.Next(), "Is nothing");
    });

    test::describe("iterate on codepoint", []() {
      auto string = UTF8::TryCreate("greg").unwrap();
      auto iter = string.Iter();

      test::equal(iter.Next(), Some(C_g));
      test::equal(iter.Next(), Some(C_r));
      test::equal(iter.Next(), Some(C_e));
      test::equal(iter.Next(), Some(C_g));

      test::ok(!iter.Next(), "Is nothing");
      test::ok(!iter.Next(), "Is nothing");
    });

    test::describe("iterate on codepoint", []() {
      auto string = UTF8::TryCreate("¼©").unwrap();
      auto iter = string.Iter();

      test::equal(iter.Next(), Some(C_34));
      test::equal(iter.Next(), Some(C_COP));

      test::ok(!iter.Next(), "Is nothing");
    });

    test::describe("iterate on arabic", []() {
      auto string = UTF8::TryCreate("سلام").unwrap();
      auto iter = string.Iter();

      test::equal(iter.Next(), Some(static_cast<uint32_t>(0x0633)));
      test::equal(iter.Next(), Some(static_cast<uint32_t>(0x0644)));
      test::equal(iter.Next(), Some(static_cast<uint32_t>(0x0627)));
      test::equal(iter.Next(), Some(static_cast<uint32_t>(0x0645)));
      test::ok(!iter.Next(), "Is nothing");
    });

    test::describe("iterate on 0xf000 range", []() {
      auto string = UTF8::TryCreate("ﬀﬁﬂﬃ").unwrap();
      auto iter = string.Iter();

      test::equal(iter.Next(), Some(static_cast<uint32_t>(0xfb00)));
      test::equal(iter.Next(), Some(static_cast<uint32_t>(0xfb01)));
      test::equal(iter.Next(), Some(static_cast<uint32_t>(0xfb02)));
      test::equal(iter.Next(), Some(static_cast<uint32_t>(0xfb03)));
      test::ok(!iter.Next(), "Is nothing");
    });
  });
}

} // namespace features::utf8
