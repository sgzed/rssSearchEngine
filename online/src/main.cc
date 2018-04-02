 ///
 /// @file    main.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-04-01 17:01:02
 ///

#include "../include/searchEngine.h"
#include "../include/MyConf.h"

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <stdlib.h>
#include <stdio.h>

#include <sstream>
using std::istringstream;
using namespace muduo;
using namespace muduo::net;

uint16_t str2u16t(const std::string& str)
{
	istringstream iss(str);
	uint16_t ret ;
	iss >> ret ;
	return ret;
}

int main()
{
	MyConf::getInstance()->show();
	
	auto confMap = MyConf::getInstance()->getConfigMap();
	
	std::string ip = confMap["ip"];
	std::string port = confMap["port"];
	std::string numThreads = confMap["threads"];

	EventLoop loop;

	InetAddress listenAddr(ip,str2u16t(port));

	LOG_INFO << "pid = " << getpid() ;

	SearchServer server(&loop,listenAddr,str2u16t(numThreads));
	
	server.start();
	
	loop.loop();
}
