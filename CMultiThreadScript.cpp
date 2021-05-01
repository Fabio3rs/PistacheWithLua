#include "CMultiThreadScript.h"
#include "CLuaFunctions.hpp"
#include "CLog.h"
#include <dirent.h>
#include "CViewsManager.h"

template<class T> void setLuaGlobal(lua_State* L, const std::string& name, const T& value)
{
	CLuaH::customParam(value).pushToLuaStack(L);
	lua_setglobal(L, name.c_str());
}

int CMultiThreadScript::view(lua_State* L)
{
	CLuaFunctions::LuaParams p(L);

	int num = p.getNumParams();

	if (num >= 1)
	{
		lua_getglobal(L, "targetString");

		int64_t i = lua_tointeger(L, -1);

		std::string& targetStr = *(std::string*)i;

		std::string viewName = lua_tostring(L, 1);

		size_t viewID = CViewsManager::view().viewIdByPathAndName("", viewName);

		if (viewID == ~((size_t)0uL))
		{
			targetStr = viewName + " 404";

			p << false;

			return p.rtn();
		}

		auto scriptViewTest = CViewsManager::view().get(viewID).instance(targetStr);

		if (scriptViewTest.luaState != nullptr)
		{
			for (int i = 2; i <= num; ++i)
			{

			}

			CLuaH::Lua().runScript(scriptViewTest);
			p << true;
		}
		else
		{
			targetStr = viewName + " 404";

			p << false;
		}
	}

	return p.rtn();
}

int CMultiThreadScript::vp(lua_State* L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 1)
	{
		lua_getglobal(L, "targetString");

		int64_t i = lua_tointeger(L, -1);

		std::string& targetStr = *(std::string*)i;

		targetStr += lua_tostring(L, 1);
	}

	return 0;
}

int CMultiThreadScript::routeRegister(lua_State* L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1) && lua_isfunction(L, 2)) {
		std::string routepath = "teste";
		routepath = lua_tostring(L, 1);

		lua_pushvalue(L, 2);
		int	fnRef = luaL_ref(L, LUA_REGISTRYINDEX);

		int routeID = 0;

		CPistacheEndpoint& svr = CPistacheEndpoint::singleton();

		//std::cout << routepath << "  " << CMultiThreadScript::scriptsMgr().acceptRouteRegister << std::endl;
		if (CMultiThreadScript::scriptsMgr().acceptRouteRegister)
		{
			CScript& script = *(CScript*)lscript(L).customPtr;

			routeID = svr.registerRoute(routepath, script.thisID);
		}
		else
		{
			CScript& script = *(CScript*)lscript(L).customPtr;

			routeID = svr.getRoute(routepath);
		}

		lscript(L).routes[routeID] = fnRef;
		lscript(L).callbacksAdded = true;
	}

	return p.rtn();
}

void CMultiThreadScript::registerControllersFunctions(lua_State* L)
{
	lua_register(L, "view", view);
	lua_register(L, "vp", vp);
	lua_register(L, "routeRegister", routeRegister);

}

std::string CMultiThreadScript::instanceAll(const std::string& path)
{
	std::string result;
	for (auto& script : scriptList)
	{
		auto testInstance = script.newInstance();

		registerControllersFunctions(testInstance.luaState);
		setLuaGlobal(testInstance.luaState, "targetString", (int64_t)&result);
		CLuaH::Lua().runScript(testInstance);

		CLuaH::Lua().runInternalEventWithParams(testInstance, "route", { path, (int64_t)&result });
	}

	return result;
}

CMultiThreadScript& CMultiThreadScript::scriptsMgr()
{
	static CMultiThreadScript multiScriptMgr;
	return multiScriptMgr;
}

std::string CMultiThreadScript::runRoute(int scriptID, size_t routeId, const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter& response)
{
	std::string result;
	auto& script = scriptList[scriptID];
	{
		auto testInstance = script.newInstance();

		testInstance.customPtr = &script;

		registerControllersFunctions(testInstance.luaState);
		setLuaGlobal(testInstance.luaState, "targetString", (int64_t)&result);

		{
			CLuaH::customParam reqcookies;
			reqcookies.setastable();

			auto& table = reqcookies.getTableData();
			for (const auto& rcookies : request.cookies())
			{
				table[rcookies.name] = rcookies.value;
			}

			reqcookies.pushToLuaStack(testInstance.luaState);
			lua_setglobal(testInstance.luaState, "_cookies");
		}

		CLuaH::Lua().runScript(testInstance);

		if (CLuaH::Lua().runInternalRouteWithParams(testInstance, routeId, { (int64_t)routeId, (int64_t)&result }))
		{
			lua_getglobal(testInstance.luaState, "set_cookies");
			CLuaH::customParam cookies;
			cookies.getFromArgs(testInstance.luaState, -1);

			for (auto& c : cookies.getTableData())
			{
				response.cookies().add({ c.first, c.second.getString() });
			}
		}
		else
		{
			// 404
		}
	}

	return result;
}

void CMultiThreadScript::loadAll()
{
	acceptRouteRegister = true;
	loadFiles("scripts");
	acceptRouteRegister = false;
}

bool CMultiThreadScript::loadFiles(const std::string& path)
{
	auto extension_from_filename = [](const std::string& fname)
	{
		size_t s;
		return std::string(((s = fname.find_first_of('.')) != fname.npos) ? (&fname.c_str()[++s]) : (""));
	};

	DIR* direntd = opendir((std::string("./") + path).c_str());
	dirent* rrd = nullptr;

	if (direntd)
	{
		rrd = readdir(direntd);
		while ((rrd = readdir(direntd)) != nullptr)
		{
			if ((rrd->d_type & DT_DIR) == 0 && extension_from_filename(rrd->d_name) == "lua")
			{
				CLog::log() << ("Loading <<" + path + "/" + rrd->d_name + ">>");

				CLog::log().SaveBuffer();

				size_t siz = scriptList.size();
				scriptList.push_back(CScript(0, CLuaH::Lua().newScript(path, rrd->d_name)));
				CLog::log().SaveBuffer();
			}
		}
		closedir(direntd);
	}

	return true;
}

CMultiThreadScript::CMultiThreadScript()
{
	acceptRouteRegister = false;
}

CLuaH::luaScript CScript::newInstance()
{
	auto clone = CLuaH::Lua().newScriptRBuffer(scriptByteCode, "clone script");
	clone.customPtr = this;
	return std::move(clone);
}

CScript::CScript()
{
	thisID = 0;

}

CScript::CScript(int id, CLuaH::luaScript&& script)
{
	thisID = id;

	mainScript = std::move(script);
	scriptByteCode = mainScript.dumpBytecode();

	script.customPtr = this;
	mainScript.customPtr = this;
	CMultiThreadScript::registerControllersFunctions(mainScript.luaState);

	CLuaH::Lua().runScript(mainScript);
}
