#include <string>

class Matcher {
 public:
  Matcher();
  static bool check(const std::string& input, const std::string& pattern);
};
