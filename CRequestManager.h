#pragma once
#ifndef _CRequestManager_H_
#define _CRequestManager_H_

#include <string>
#include <vector>
#include "CCache.h"

struct routeResponse;

struct requestStructure
{
	//CServerHTTP::HTMLHeader &getHeader;
	//CServerHTTP::HTMLHeader &responseHeader;
	std::vector<char> &data;
	routeResponse &rr;

	/*requestStructure(CServerHTTP::HTMLHeader &getHeader, CServerHTTP::HTMLHeader &responseHeader, std::vector<char> &data, routeResponse &rr) :
		getHeader(getHeader), responseHeader(responseHeader), data(data), rr(rr)
	{

	}*/
};

class CRequestManager
{

public:
	struct URL
	{
		std::string path;

		// TODO params
	};

	URL processUrl(const char *url);
	URL processUrl(std::string &url);

	void executeURL(URL &url, CCache::cacheInstance *cache, CCache::cacheResult &cr);

	bool executeURLNoCache(URL &url, requestStructure &rs);

	static CRequestManager &request();

private:
	CRequestManager();
	CRequestManager(CRequestManager&) = delete;
};

#endif // !_CRequestManager_H_
