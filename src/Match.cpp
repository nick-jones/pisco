#include "Match.h"

using namespace std;

/*
 * Glob-style match function. It determines whether a pattern satisfies a string.
 */
bool Matcher::check(const string& input, const string& pattern) {
  const char wildcard = '*';
  unsigned long input_pos = 0;
  unsigned long input_len = input.length();
  unsigned long pattern_pos = 0;
  unsigned long pattern_len = pattern.length();

  // Loop throught matching input and pattern characters
  while (input_pos < input_len && pattern_pos < pattern_len && pattern[pattern_pos] != wildcard
          && pattern[pattern_pos] == input[input_pos]) {
    // Increment input and pattern position pointers, as they match
    input_pos++;
    pattern_pos++;
  }

  // If the end of the pattern is a wildcard, and the pattern position is 1 character off, then the pattern matches
  if (pattern_pos+1 == pattern_len && pattern[pattern_pos] == wildcard) {
    return true;
  }

  // If the entire input string has been read through, then the pattern matches if it has been entirely read too
  if (input_pos == input_len) {
    return pattern_pos == pattern_len;
  }

  // If the pattern hasn't reached a wildcard character, then it there are character mismatches
  if (pattern_pos == pattern_len || pattern[pattern_pos] != '*') {
    return false;
  }

  // Shift past the wildcard character
  pattern_pos++;

  bool matched = false;
  unsigned int input_check_start_pos;
  unsigned int input_check_pos;
  unsigned int pattern_check_pos;

  while (input_pos < input_len && !matched) {
    // Scroll forward to find a character that matches the first character after the wildcard
    if (input[input_pos] != pattern[pattern_pos]) {
      input_pos++;
      continue;
    }

    // Once there is a match, check the subsequent characters
    pattern_check_pos = pattern_pos + 1;
    input_check_pos = input_check_start_pos = input_pos + 1;

    while (input_check_pos < input_len && pattern_check_pos < pattern_len
            && input[input_check_pos] == pattern[pattern_check_pos] && pattern[pattern_check_pos] != wildcard) {
      // Increment the check input and pattern positions, as they match
      input_check_pos++;
      pattern_check_pos++;
    }

    if ((pattern_check_pos == pattern_len && input_check_pos == input_len)
          || (pattern_check_pos+1 == pattern_len && pattern[pattern_check_pos] == '*')) {
       // If the end of the pattern is reached, and the input has been consumed, then the pattern matches
       matched = true;
    }
    else if (pattern[pattern_check_pos] == wildcard) {
      // Another wildcard has been reached
      input_pos = input_check_pos;
      pattern_pos = pattern_check_pos + 1;
    }
    else {
      // Non match, continue from +1 of the previous position
      input_pos = input_check_start_pos;
    }
  }

  return matched;
}
