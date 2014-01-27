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

service Search {
  Result lookup(1: string pattern, 2: i32 limit = 10, 3: bool total_required = false),
  bool add(1: Item item),
  bool replace(1: Item item),
  bool remove(1: i32 id)
}
