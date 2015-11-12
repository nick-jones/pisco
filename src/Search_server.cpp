#include "gen-cpp/Search.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/algorithm/string/join.hpp>
#include "Database.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;
using boost::algorithm::join;

using namespace ::pisco::thrift;

class SearchHandler : virtual public SearchIf
{
 private:
  shared_ptr<Database> database;

 public:
  SearchHandler(const shared_ptr<Database> d) : database(d)
  {
  }

  void lookup(Result& _return, const Query& query)
  {
    std::cout << "Lookup: " <<  query.pattern << "\n";

    database->lookup(_return, query);
  }

  void lookupAdvanced(Result& _return, const AdvancedQuery& query)
  {
    std::string includes = join(query.include_patterns, ", ");
    std::string excludes = join(query.exclude_patterns, ", ");
    std::cout << "Advanced Lookup: " <<  includes << " ! " << excludes << "\n";

    database->lookupAdvanced(_return, query);
  }

  bool add(const Item& item)
  {
    std::cout << "Add: " << item.id << "," << item.value << "," << item.time << "\n";

    return database->add(item);
  }

  bool replace(const Item& item)
  {
    std::cout << "Replace: " << item.id << "\n";

    database->remove(item.id);
    return database->add(item);
  }

  bool remove(const int32_t id)
  {
    std::cout << "Remove: " << id << "\n";

    return database->remove(id);
  }
};

int main(int argc, char **argv)
{
  int port = 9090;

  shared_ptr<Database> database(new Database());
  shared_ptr<SearchHandler> handler(new SearchHandler(database));
  shared_ptr<TProcessor> processor(new SearchProcessor(handler));
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(100);
  shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  threadManager->threadFactory(threadFactory);
  threadManager->start();

  TNonblockingServer server(processor, protocolFactory, port, threadManager);
  server.serve();

  return 0;
}

