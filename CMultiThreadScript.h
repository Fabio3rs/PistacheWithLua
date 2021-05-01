#pragma once
#ifndef _CMultiThreadScript_h_
#define _CMultiThreadScript_h_

#include <unordered_map>
#include <map>
#include <vector>
#include <deque>
#include <cmath>
#include <mutex>
#include <atomic>
#include <thread>
#include "CMultiThreadPool.h"
#include "CLuaH.hpp"
#include "CPistacheEndpoint.hpp"

class CScript
{
	friend class CMultiThreadScript;
	std::vector<char> scriptByteCode;
	int thisID;

	CLuaH::luaScript mainScript;

public:
	CLuaH::luaScript newInstance();

	CScript();
	CScript(int id, CLuaH::luaScript&& script);
};

/*
* Criar um manager de calls externos
* ScriptsState terá uma nova instância a cada request
* etc.
*
*/

class ScriptsState
{

public:
	ScriptsState();
};

class CMultiThreadScript
{
	std::deque<CScript> scriptList;
	friend CScript;

	static int view(lua_State* L);

	static int vp(lua_State* L);

	static int routeRegister(lua_State* L);

	static void registerControllersFunctions(lua_State* L);

	bool acceptRouteRegister;

public:

	std::string instanceAll(const std::string& path);

	static CMultiThreadScript& scriptsMgr();

	std::string runRoute(int scriptID, size_t routeId, const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter& response);

	void loadAll();

private:
	bool loadFiles(const std::string& path);

	CMultiThreadScript();
	CMultiThreadScript(CMultiThreadScript&) = delete;
};

#endif
