#include "gen-cpp/Search.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "Database.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

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
    database->lookup(_return, query);
  }

  void lookupAdvanced(Result& _return, const AdvancedQuery& query)
  {
    database->lookupAdvanced(_return, query);
  }

  bool add(const Item& item)
  {
    return database->add(item);
  }

  bool replace(const Item& item)
  {
    database->remove(item.id);
    return database->add(item);
  }

  bool remove(const int32_t id)
  {
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

