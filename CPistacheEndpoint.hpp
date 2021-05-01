#pragma once
#ifndef _CPistacheEndpoint_hpp
#define _CPistacheEndpoint_hpp

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <atomic>
#include <map>
#include <string>

class LuaRoute
{
	int script, route;

public:
	void operator()(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

	inline LuaRoute(int scriptID, int routeID) : script(scriptID), route(routeID)
	{

	}
};

class CPistacheEndpoint
{
	std::atomic<int> registeredRoutes;
	std::map<std::string, int> routelisttmp;

public:
	int registerRoute(std::string url, int script);
	int getRoute(std::string url) const;

	void init(Pistache::Address addr, size_t thr = 2);
	void start();

	static CPistacheEndpoint& singleton();

private:
	void setupRoutes();
	void staticfile(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

	CPistacheEndpoint() noexcept;
	CPistacheEndpoint(CPistacheEndpoint&) = delete;

	std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
	Pistache::Rest::Router router;
};

#endif

