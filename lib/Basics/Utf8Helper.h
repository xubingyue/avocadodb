////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 AvocadoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is AvocadoDB GmbH, Cologne, Germany
///
/// @author Frank Celler
/// @author Achim Brandt
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_BASICS_UTF8HELPER_H
#define ARANGODB_BASICS_UTF8HELPER_H 1

#include "Basics/Common.h"

#include <unicode/coll.h>
#include <unicode/regex.h>
#include <unicode/ustring.h>

namespace avocadodb {
namespace basics {

class Utf8Helper {
  Utf8Helper(Utf8Helper const&) = delete;
  Utf8Helper& operator=(Utf8Helper const&) = delete;

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief a default helper
  //////////////////////////////////////////////////////////////////////////////

  static Utf8Helper DefaultUtf8Helper;

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief constructor
  /// @param lang   Lowercase two-letter or three-letter ISO-639 code.
  ///     This parameter can instead be an ICU style C locale (e.g. "en_US")
  //////////////////////////////////////////////////////////////////////////////

  Utf8Helper(std::string const& lang, void *icuDataPtr);

  explicit Utf8Helper(void *icuDataPtr);

  ~Utf8Helper();

 public:
  //////////////////////////////////////////////////////////////////////////////
  ///  public functions
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// @brief compare utf8 strings
  /// -1 : left < right
  ///  0 : left == right
  ///  1 : left > right
  //////////////////////////////////////////////////////////////////////////////

  int compareUtf8(char const* left, char const* right) const;

  int compareUtf8(char const* left, size_t leftLength, char const* right,
                  size_t rightLength) const;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief compare utf16 strings
  /// -1 : left < right
  ///  0 : left == right
  ///  1 : left > right
  //////////////////////////////////////////////////////////////////////////////

  int compareUtf16(uint16_t const* left, size_t leftLength,
                   uint16_t const* right, size_t rightLength) const;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief set collator by language
  /// @param lang   Lowercase two-letter or three-letter ISO-639 code.
  ///     This parameter can instead be an ICU style C locale (e.g. "en_US")
  /// @param icuDataPointer data file to be loaded by the application
  //////////////////////////////////////////////////////////////////////////////

  bool setCollatorLanguage(std::string const& lang, void* icuDataPointer);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief get collator language
  //////////////////////////////////////////////////////////////////////////////

  std::string getCollatorLanguage();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief get collator country
  //////////////////////////////////////////////////////////////////////////////

  std::string getCollatorCountry();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Lowercase the characters in a UTF-8 string.
  //////////////////////////////////////////////////////////////////////////////

  std::string toLowerCase(std::string const& src);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Lowercase the characters in a UTF-8 string.
  //////////////////////////////////////////////////////////////////////////////

  char* tolower(TRI_memory_zone_t* zone, char const* src, int32_t srcLength,
                int32_t& dstLength);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Uppercase the characters in a UTF-8 string.
  //////////////////////////////////////////////////////////////////////////////

  std::string toUpperCase(std::string const& src);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Uppercase the characters in a UTF-8 string.
  //////////////////////////////////////////////////////////////////////////////

  char* toupper(TRI_memory_zone_t* zone, char const* src, int32_t srcLength,
                int32_t& dstLength);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the words of a UTF-8 string.
  //////////////////////////////////////////////////////////////////////////////

  bool tokenize(std::set<std::string>& words, std::string const& text,
                size_t minimalWordLength, size_t maximalWordLength,
                bool lowerCase);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief builds a regex matcher for the specified pattern
  //////////////////////////////////////////////////////////////////////////////

  RegexMatcher* buildMatcher(std::string const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not value matches a regex
  //////////////////////////////////////////////////////////////////////////////

  bool matches(RegexMatcher*, char const* pattern, size_t patternLength, 
               bool partial, bool& error);

  std::string replace(RegexMatcher*, char const* pattern, size_t patternLength,
                      char const* replacement, size_t replacementLength,
                      bool partial, bool& error);

 private:
  Collator* _coll;
};
}
}

////////////////////////////////////////////////////////////////////////////////
/// @brief convert a utf-8 string to a uchar (utf-16)
////////////////////////////////////////////////////////////////////////////////

UChar* TRI_Utf8ToUChar(TRI_memory_zone_t* zone, char const* utf8,
                       size_t inLength, size_t* outLength);

////////////////////////////////////////////////////////////////////////////////
/// @brief convert a uchar (utf-16) to a utf-8 string
////////////////////////////////////////////////////////////////////////////////

char* TRI_UCharToUtf8(TRI_memory_zone_t* zone, UChar const* uchar,
                      size_t inLength, size_t* outLength);

////////////////////////////////////////////////////////////////////////////////
/// @brief normalize an utf8 string (NFC)
////////////////////////////////////////////////////////////////////////////////

char* TRI_normalize_utf8_to_NFC(TRI_memory_zone_t* zone, char const* utf8,
                                size_t inLength, size_t* outLength);

////////////////////////////////////////////////////////////////////////////////
/// @brief normalize an utf16 string (NFC) and export it to utf8
////////////////////////////////////////////////////////////////////////////////

char* TRI_normalize_utf16_to_NFC(TRI_memory_zone_t* zone, uint16_t const* utf16,
                                 size_t inLength, size_t* outLength);

////////////////////////////////////////////////////////////////////////////////
/// @brief compare two utf8 strings
////////////////////////////////////////////////////////////////////////////////

static inline int TRI_compare_utf8(char const* left, char const* right) {
  TRI_ASSERT(left != nullptr);
  TRI_ASSERT(right != nullptr);
  return avocadodb::basics::Utf8Helper::DefaultUtf8Helper.compareUtf8(left, right);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief compare two utf8 strings
////////////////////////////////////////////////////////////////////////////////

static inline int TRI_compare_utf8(char const* left, size_t leftLength, 
                                   char const* right, size_t rightLength) {
  TRI_ASSERT(left != nullptr);
  TRI_ASSERT(right != nullptr);
  return avocadodb::basics::Utf8Helper::DefaultUtf8Helper.compareUtf8(left, leftLength, 
                                                                     right, rightLength);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Lowercase the characters in a UTF-8 string
////////////////////////////////////////////////////////////////////////////////

char* TRI_tolower_utf8(TRI_memory_zone_t* zone, char const* src,
                       int32_t srcLength, int32_t* dstLength);

#endif