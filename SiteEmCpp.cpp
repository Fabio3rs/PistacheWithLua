// SiteEmCpp.cpp : Defines the entry point for the console application.
//

#include <cstdio>
#include <chrono>
#include <string>
#include <iostream>
#include "CLog.h"
#include "CLuaH.hpp"
#include "CLuaFunctions.hpp"
#include "CViewsManager.h"
#include "CMultiThreadScript.h"
#include "CPistacheEndpoint.hpp"


using namespace Pistache;
void printCookies(const Http::Request& req)
{
	auto cookies = req.cookies();
	std::cout << "Cookies: [" << std::endl;
	const std::string indent(4, ' ');
	for (const auto& c : cookies)
	{
		std::cout << indent << c.name << " = " << c.value << std::endl;
	}
	std::cout << "]" << std::endl;
}

int main(int argc, char* argv[], char* envp[])
{
	CLog::log() << "Server.log";

	CViewsManager::view();
	CMultiThreadScript::scriptsMgr().loadAll();

	CLog::log().SaveBuffer();

	Port port(9080);

	int thr = 2;

	if (argc >= 2)
	{
		port = static_cast<uint16_t>(std::stol(argv[1]));

		if (argc == 3)
			thr = std::stoi(argv[2]);
	}

	Address addr(Ipv4::any(), port);

	std::cout << "Cores = " << std::thread::hardware_concurrency() << std::endl;
	std::cout << "Using " << thr << " threads" << std::endl;

	CPistacheEndpoint& stats = CPistacheEndpoint::singleton();

	stats.init(addr, thr);
	stats.start();

	return 0;
}

