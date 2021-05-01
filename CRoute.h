#pragma once
#ifndef _CRoute_h_
#define _CRoute_h_

#include <string>
#include <deque>
#include <vector>
#include <cstring>
#include <cstdlib>
#include "CRequestManager.h"

class Route
{
	friend class CRoute;

public:
	struct urlRule
	{
		std::string text;
		bool optional, isParameter;

		inline urlRule()
		{
			optional = false;
			isParameter = false;
		}
	};

private:
	std::string routeRuleStr;

	bool isCacheable, isCached;
	bool isScript;
	bool acceptPost;

	std::vector<int> scriptsMiddleware;
	int scriptController;

	std::vector<urlRule> rules;

	void parseUrlToRules(const std::string& url);

public:
	CRequestManager::URL url;

	inline const std::string& ruleStr()
	{
		return routeRuleStr;
	}

	inline bool getIsCacheableFlag()
	{
		return isCacheable;
	}

	inline bool getIsCachedFlag()
	{
		return isCached;
	}

	inline bool getIsScriptFlag()
	{
		return isScript;
	}

	inline bool getScript()
	{
		return scriptController;
	}

	inline bool getAcceptPostFlag()
	{
		return acceptPost;
	}

	inline bool hasMiddleware()
	{
		return scriptsMiddleware.size() > 0;
	}

	bool compare(const std::string& url, std::vector<std::string>& urlParts);

	Route();
};

struct routeResponse
{
	Route* route;
	int routeId;
	int httpCode;
	bool responseNow;

	std::vector<std::string> r;

	inline routeResponse()
	{
		route = nullptr;
		routeId = -1;
		httpCode = 404;
		responseNow = false;
	}
};

class CRoute
{
	std::deque<Route> routeList;
public:

	static CRoute& routes();

	size_t registerRoute(const std::string& url, int script);
	size_t getRoute(const std::string& urlRule);

	routeResponse testRoute(const std::string& url, const std::string& method);

private:
	CRoute();
	CRoute(CRoute&) = delete;
};

#endif
