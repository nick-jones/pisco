#include <string>
#include <set>

class Matcher {
 public:
  Matcher();
  static bool check(const std::string& input, const std::string& pattern);
  static bool check(const std::string& input, const std::set<std::string>& inclusion_patterns,
                    const std::set<std::string>& exclusion_patterns);
};
