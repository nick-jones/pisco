namespace cpp pisco.thrift
namespace php Pisco.Thrift

struct Item {
  1: required i32 id,
  2: required i32 time,
  3: optional string value,
  4: optional bool deleted
}

struct Result {
  1: required list<i32> ids,
  2: optional i32 total = 0
}

struct Query {
  1: optional set<string> include_patterns,
  2: optional set<string> exclude_patterns,
  3: optional i32 limit = 10,
  4: optional bool total_required = false,
  5: optional bool reverse_order = false,
  6: optional bool included_deleted = false
}

service Search {
  Result lookup(1: Query query),
  bool add(1: Item item),
  bool replace(1: Item item),
  bool remove(1: i32 id)
}
