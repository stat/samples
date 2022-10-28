#include "ib_client/client.h"
#include "ib_client/dispatcher.h"

//
// Class
//

//
// Constructor

IBClient::IBClient(IBClientDispatcher *dispatcher) :
  extraAuth(false),
  dispatcher(dispatcher),
  session(new IBClientSession()),

  socketSignal(2000),
  socketReader(NULL),
  socketHandler(new IBClientHandler(this, session, dispatcher)),
  socket(new EClientSocket(socketHandler, &socketSignal))
{
  messageThread = pthread_self();
  messageThreadRunning = true;
}

//
// Destructor

IBClient::~IBClient()
{
  if(!pthread_equal(pthread_self(), messageThread))
  {
    messageThreadRunning = false;
    pthread_join(messageThread, NULL);
  }

  if(socketReader)
  {
    delete socketReader;
  }

  delete socket;
  delete socketHandler;

  delete session;
}

//
// Message Thread
//

void IBClient::readMessages()
{
  while(messageThreadRunning)
  {
    socketSignal.waitForSignal();
    socketReader->processMsgs();
  }
}

void * IBClient::readMessagesThread(void *context)
{
  reinterpret_cast<IBClient *>(context)->readMessages();
  return 0;
}

//
// Network
//

bool IBClient::connect(const char *host, int port, int clientId)
{
#ifdef IB_CLIENT_DEBUG
  printf(
    "> Connecting to %s:%d clientId:%d\n",
    !( host && *host) ? "127.0.0.1" : host, port, clientId
  );
#endif

	bool res = socket->eConnect(host, port, clientId, extraAuth);

  if(!res)
  {
    return false;
  }
  
#ifdef IB_CLIENT_DEBUG
  printf(
    "> Connected to %s:%d clientId:%d\n",
    socket->host().c_str(), socket->port(), clientId
  );
#endif

  socketReader = new EReader(socket, &socketSignal);
  socketReader->start();

  pthread_create(&messageThread, NULL, readMessagesThread, this);

  return true;
}

bool IBClient::connected() const
{
  return socket->isConnected();
}

bool IBClient::disconnect() const
{
  socket->eDisconnect(true);

  return true;
}
