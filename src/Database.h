#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <utility>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "Match.h"
#include "gen-cpp/Search.h"

class Database {
 private:
  typedef boost::shared_ptr<pisco::thrift::Item> item_shared_pointer;
  typedef std::unordered_map<int32_t, item_shared_pointer>::iterator ids_iterator;
  typedef std::multimap<int32_t, item_shared_pointer>::iterator store_iterator;
  typedef std::multimap<int32_t, item_shared_pointer>::reverse_iterator store_reverse_iterator;

  std::multimap<int32_t, item_shared_pointer> store;
  std::unordered_map<int32_t, item_shared_pointer> ids;

  store_iterator locate_item(const int32_t time, const int32_t id);

 public:
  Database();
  bool add(const pisco::thrift::Item& item);
  bool remove(const int32_t id);
  void lookup(pisco::thrift::Result& result, const pisco::thrift::Query& query);
};
