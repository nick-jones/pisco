#include "Match.h"

using namespace std;

/*
 * Glob-style matching function, which accepts sets of inclusion and exclusion patterns.
 */
bool Matcher::check(const string& input, const set<string>& include, const set<string>& exclude) {

  set<string>::const_iterator it;

  for (it = include.begin(); it != include.end(); ++it) {
    if (!Matcher::check(input, *it)) {
      return false;
    }
  }

  for (it = exclude.begin(); it != exclude.end(); ++it) {
    if (Matcher::check(input, *it)) {
      return false;
    }
  }

  return true;
}

/*
 * Glob-style matching function, only wildcard characters are currently supported. This function determines whether
 * a given pattern satisfies a string. It uses a fairly naïve bruteforce approach to achieve this.
 */
bool Matcher::check(const string& input, const string& pattern) {

  const char wildcard = '*';

  unsigned long input_pos = 0;
  unsigned long pattern_pos = 0;

  unsigned long input_len = input.length();
  unsigned long pattern_len = pattern.length();

  bool trailing_wildcard = pattern[pattern_len - 1] == wildcard;
  bool leading_wildcard = pattern[0] == wildcard;

  // Final wildcard is irrelevant, as we're only interested in whether the text up until it has matched
  if (trailing_wildcard) {
    pattern_len--;
  }

  // Scroll through initial matching characters, if there is no leading wildcard character
  if (!leading_wildcard) {
    while (input_pos < input_len && pattern_pos < pattern_len
            && input[input_pos] == pattern[pattern_pos]
            && pattern[pattern_pos] != wildcard) {
      // Shift the positions forward
      input_pos++;
      pattern_pos++;
    }

    // All initial pattern characters must have matched
    if (pattern[pattern_pos] != wildcard && (input_pos != input_len || pattern_pos != pattern_len)) {
      return false;
    }
  }

  // Check whether a match has occurred yet
  bool matched = pattern_pos++ == pattern_len && (input_pos == input_len || trailing_wildcard);

  unsigned long input_check_pos;
  unsigned long pattern_check_pos;

  while (input_pos < input_len && pattern_pos < pattern_len && !matched) {
    // Look for the first character of the current pattern segment in the input
    if (input[input_pos] != pattern[pattern_pos]) {
      input_pos++;
      continue;
    }

    // First character matches, check the subsequent characters
    input_check_pos = input_pos + 1;
    pattern_check_pos = pattern_pos + 1;

    while (input_check_pos < input_len && pattern_check_pos < pattern_len
            && input[input_check_pos] == pattern[pattern_check_pos]
            && pattern[pattern_check_pos] != wildcard) {
      // Increment the positions
      input_check_pos++;
      pattern_check_pos++;
    }

    if (pattern_check_pos == pattern_len && (input_check_pos == input_len || trailing_wildcard)) {
      // A match has occurred if the entire pattern has been consumed along with the input
      matched = true;
    }
    else if (pattern[pattern_check_pos] == wildcard) {
      // Another wildcard has been hit, so this segment is covered; move onto the next
      input_pos = input_check_pos;
      pattern_pos = pattern_check_pos + 1;
    }
    else {
      // Look for another occurance of the pattern segments first character in the input
      input_pos++;
    }
  }

  return matched;
}
