///
/// @file    searchEngine.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-04-01 16:47:56
///

#include "../include/searchEngine.h"
#include "../include/WordQuery.h"
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include <assert.h>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

SearchServer::SearchServer(EventLoop* loop,const InetAddress& listenAddr,unsigned short numThreads)
 	:_server(loop,listenAddr,"SearchServer")
	 ,_numThreads(numThreads)
	  ,_threadpool("")
	  ,_wordQuery()
{
	_server.setConnectionCallback(std::bind(&SearchServer::onConnection,this,_1));

	_server.setMessageCallback(std::bind(&SearchServer::onMessage,this,_1,_2,_3));

}

void SearchServer::start()
{
	_threadpool.start(_numThreads);
	_server.start();
}

void SearchServer::onConnection(const TcpConnectionPtr& conn)
{
	LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
		<< conn->localAddress().toIpPort() << " is "
		<< (conn->connected() ? "UP" : "DOWN");
}


void SearchServer::onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time)
{
	//string msg(buf->retrieveAllAsString());

	std::string msg(buf->peek(),buf->readableBytes());

	size_t pos = msg.find("\n");

	buf->retrieveUntil(buf->peek()+pos+1);

	msg = msg.substr(0,pos);

	 LOG_INFO << conn->name() << " echo " << msg.size()
		    << " bytes at " << time.toString() << " msg is " << msg ;

	_threadpool.run(std::bind(&SearchServer::doTaskThread,this,conn,msg));

}


void SearchServer::doTaskThread(const TcpConnectionPtr& conn,const std::string& msg)
{
	std::string ret = _wordQuery.doQuery(msg);	
	int  sz = ret.size();
	LOG_INFO << "reply " << sz << " bytes ";
	conn->send(ret);
	conn->shutdown();
}














