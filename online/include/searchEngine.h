 ///
 /// @file    searchEngine.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-04-01 16:33:36
 ///

#ifndef __WD_SEARCHENGINE_H__
#define __WD_SEARCHENGINE_H__

#include <muduo/net/TcpServer.h>
#include <muduo/base/ThreadPool.h>
#include "../include/WordQuery.h"

class SearchServer
{
public:
	SearchServer(muduo::net::EventLoop* loop,
			const muduo::net::InetAddress& listenAddr,
			unsigned short numThreads);

	void start();

private:
	void onConnection(const muduo::net::TcpConnectionPtr& coon);

	void onMessage(const muduo::net::TcpConnectionPtr& conn,
			muduo::net::Buffer* buf,muduo::Timestamp time);

	void doTaskThread(const muduo::net::TcpConnectionPtr & conn,const string& msg);
private:
	muduo::net::TcpServer _server;
	unsigned short _numThreads;
	muduo::ThreadPool _threadpool;
	WordQuery _wordQuery;	
};

#endif
