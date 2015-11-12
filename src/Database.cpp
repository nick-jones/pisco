#include <stdio.h>
#include "Database.h"

using namespace ::pisco::thrift;
using namespace std;
using boost::shared_ptr;
using boost::make_shared;

Database::Database()
{
}

/*
 * Add an item into storage. The item is currently indexed by ID and time.
 */
bool Database::add(const Item& item)
{
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
 * Lookup an item by a single pattern. If total_required is FALSE, then no total
 * count is returned; this affords quicker lookup, as it may be the case that
 * the entire set of values need be scanned.
 */
void Database::lookup(Result& result, const Query& query)
{
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
 * Lookup an item by multiple inclusion and exclusion patterns. Also provides
 * the ability to filter by attributes and include deleted items.
 */
void Database::lookupAdvanced(Result& result, const AdvancedQuery& query)
{
  store_reverse_iterator it_store;
  set<string> include_patterns = query.include_patterns;
  set<string> exclude_patterns = query.exclude_patterns;
  map<string, bool> with_attributes = query.with_attributes;
  bool check_attributes = with_attributes.size() > 0;
  bool total_required = query.total_required;
  bool non_deleted_only = !query.include_deleted;
  uint32_t limit = query.limit;

  for (it_store = store.rbegin(); it_store != store.rend(); ++it_store) {
    Item& item = *(it_store->second);

    if (non_deleted_only && item.deleted) {
      continue;
    }

    if (check_attributes && !this->itemHasAttributes(item, with_attributes)) {
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
 * Indicates whether the supplied item holds *all* of the supplied attributes
 * by key & value.
 */
bool Database::itemHasAttributes(const Item& item, map<string, bool> attributes)
{
  map<string, bool> item_attributes = item.attributes;
  attribute_iterator it;
  attribute_iterator result;
  attribute_iterator miss = item_attributes.end();

  for (it = attributes.begin(); it != attributes.end(); ++it) {
    result = item_attributes.find(it->first);

    if (result == miss || result->second != it->second) {
      return false;
    }
  }

  return true;
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