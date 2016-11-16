#include <XSMBServiceHandler.h>
#include <FileShareServiceHandler.h>

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/protocol/TMultiplexedProtocol.h"
#include "thrift/TProcessor.h"

#include "thrift/server/TNonblockingServer.h"
#include "thrift/concurrency/PlatformThreadFactory.h"

#include "thrift/transport/TServerSocket.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/processor/TMultiplexedProcessor.h"

using namespace azure::storage::cpp_linux;

using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;


int run_server(int port, unsigned int threadCount);