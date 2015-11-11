#include <stdio.h>
#include "Database.h"
#include <boost/algorithm/string/join.hpp>

using namespace ::pisco::thrift;
using namespace std;
using boost::shared_ptr;
using boost::make_shared;
using boost::algorithm::join;

Database::Database()
{
}

/*
 * Add an item into storage. The item is currently indexed by ID and time.
 */
bool Database::add(const Item& item)
{
  printf("add: %d, %s, %d, %d\n", item.id, item.value.c_str(), item.time, item.deleted);

  bool result = false;
  pair<ids_iterator, bool> insert_res;

  item_shared_pointer item_ptr(make_shared<Item>(item));

  // Insert a mapping of ID -> item pointer
  insert_res = ids.insert(pair<int32_t, item_shared_pointer>(item.id, item_ptr));

  // If the ID was not a duplicate
  if (insert_res.second) {
    // Insert a mapping of time -> item pointer
    store.insert(store.end(), pair<int32_t, item_shared_pointer>(item.time, item_ptr));
    result = true;
  }

  return result;
}

/*
 * Remove an item from storage, by ID.
 */
bool Database::remove(const int32_t id)
{
  printf("remove: %d\n", id);

  bool result = false;
  ids_iterator ids_result = ids.find(id);

  if (ids_result != ids.end()) {
    Item& item = *(ids_result->second);
    store_iterator store_result = this->locateItem(item.time, item.id);

    // Remove from time map
    if (store_result != store.end()) {
      store.erase(store_result);
    }

    // Remove from IDs map
    ids.erase(ids_result);
    result = true;
  }

  return result;
}

/*
 * Lookup an item by a single pattern.
 */
void Database::lookup(Result& result, const Query& query)
{
  printf("lookup: %s\n", query.pattern.c_str());

  store_reverse_iterator it;
  string pattern = query.pattern;
  bool total_required = query.total_required;
  uint32_t limit = query.limit;

  for (it = store.rbegin(); it != store.rend(); ++it) {
    Item& item = *(it->second);

    if (item.deleted || !Matcher::check(item.value, pattern)) {
      continue;
    }

    if (!this->pushResult(result, item.id, limit, total_required)) {
      break;
    }
  }
}

/*
 * Lookup an item by patterns. Limit can be supplied to restrict the results size. If total_required is FALSE, then
 * no total count is supplied; this affords quicker lookup, as it may be the case that the entire set of values
 * need be scanned.
 */
void Database::lookupAdvanced(Result& result, const AdvancedQuery& query)
{
  string includes_flat = join(query.include_patterns, ", ");
  string excludes_flat = join(query.exclude_patterns, ", ");
  printf("lookupAdvanced: %s ~ %s\n", includes_flat.c_str(), excludes_flat.c_str());

  store_reverse_iterator it;
  set<string> include_patterns = query.include_patterns;
  set<string> exclude_patterns = query.exclude_patterns;
  bool total_required = query.total_required;
  bool include_deleted = query.include_deleted;
  uint32_t limit = query.limit;

  for (it = store.rbegin(); it != store.rend(); ++it) {
    Item& item = *(it->second);

    if (item.deleted && !include_deleted) {
      continue;
    }

    if (!Matcher::check(item.value, include_patterns, exclude_patterns)) {
      continue;
    }

    if (!this->pushResult(result, item.id, limit, total_required)) {
      break;
    }
  }
}

/*
 * Pushes an ID into a result set. Return bool indicates whether or not further
 * items should be added.
 */
bool Database::pushResult(Result& result, int32_t id, int32_t limit, bool total_required)
{
  result.total++;
  unsigned long results_size = result.ids.size();

  if (results_size < (unsigned)limit) {
    result.ids.push_back(id);
  }

  if (!total_required && results_size+1 == (unsigned)limit) {
    return false;
  }

  return true;
}

/*
 * Locate an item in the "store" multimap by time and ID.
 */
Database::store_iterator Database::locateItem(const int32_t time, const int32_t id)
{
  // Find all items with the appropriate time
  pair<store_iterator, store_iterator> iterpair = store.equal_range(time);
  store_iterator result = store.end();
  store_iterator it;

  // Scan through all items to find the appropriate item by ID.
  for (it = iterpair.first; it != iterpair.second; ++it) {
    Item& item = *(it->second);

    if (item.id == id) {
      result = it;
      break;
    }
  }

  return result;
}