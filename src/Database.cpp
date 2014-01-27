#include <stdio.h>
#include "Database.h"

using namespace ::pisco::thrift;
using namespace std;
using boost::shared_ptr;
using boost::make_shared;

Database::Database() { }

/*
 * Add an item into storage. The item is currently indexed by ID and time.
 */
bool Database::add(const Item& item) {

  printf("Add: %d, %s, %d, %d\n", item.id, item.value.c_str(), item.time, item.deleted);

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
bool Database::remove(const int32_t id) {

  printf("Remove: %d\n", id);

  bool result = false;
  ids_iterator ids_result = ids.find(id);

  if (ids_result != ids.end()) {
    Item& item = *(ids_result->second);
    store_iterator store_result = this->locate_item(item.time, item.id);

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
 * Lookup an item by pattern. Limit can be supplied to restrict the results size. If total_required is FALSE, then
 * no total count is supplied; this affords quicker lookup, as it may be the case that the entire set of values
 * need be scanned.
 */
void Database::lookup(Result& result, const string& pattern, const uint32_t limit, const bool total_required) {

  printf("Lookup: %s (limit: %d, total_required: %d)\n", pattern.c_str(), limit, total_required);

  unsigned long results_size;
  store_reverse_iterator it;

  for (it = store.rbegin(); it != store.rend(); ++it) {
    Item& item = *(it->second);

    if (item.deleted || !Matcher::check(item.value, pattern)) {
      continue;
    }

    result.total++;
    results_size = result.ids.size();

    if (results_size < limit) {
      result.ids.push_back(item.id);
    }

    if (!total_required && results_size+1 == limit) {
      break;
    }
  }
}

/*
 * Locate an item in the "store" multimap by time and ID.
 */
Database::store_iterator Database::locate_item(const int32_t time, const int32_t id) {
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
