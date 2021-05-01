#include "CPistacheEndpoint.hpp"
#include "CMultiThreadScript.h"

CPistacheEndpoint::CPistacheEndpoint() noexcept
{

}

CPistacheEndpoint& CPistacheEndpoint::singleton()
{
    static CPistacheEndpoint instance;
    return instance;
}

void LuaRoute::operator()(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    CMultiThreadScript& mgr = CMultiThreadScript::scriptsMgr();

    std::string result = mgr.runRoute(script, route);
    response.send(Pistache::Http::Code::Ok, result);
}

void CPistacheEndpoint::init(Pistache::Address addr, size_t thr)
{
    httpEndpoint = std::make_shared<Pistache::Http::Endpoint>(addr);

    auto opts = Pistache::Http::Endpoint::options()
        .threads(static_cast<int>(thr));
    httpEndpoint->init(opts);
    setupRoutes();
}

int CPistacheEndpoint::registerRoute(std::string url, int script)
{
    using namespace Pistache;
    using namespace Pistache::Rest;

    int id = registeredRoutes.fetch_add(1);
    routelisttmp[url] = id;
    Routes::Get(router, url, Routes::bind(&LuaRoute::operator(), std::make_shared<LuaRoute>(script, id)));
    return id;
}

int CPistacheEndpoint::getRoute(std::string url) const
{
    auto it = routelisttmp.find(url);

    if (it == routelisttmp.end())
        return 0;

    return it->second;
}

void CPistacheEndpoint::start()
{
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
}

void CPistacheEndpoint::setupRoutes()
{
    using namespace Pistache;
    using namespace Pistache::Rest;

    Routes::NotFound(router, Routes::bind(&CPistacheEndpoint::staticfile, this));
}

void CPistacheEndpoint::staticfile(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    if (request.resource().compare(0, sizeof("/static/") - 1, "/static/") == 0 && request.method() == Pistache::Http::Method::Get)
    {
        Pistache::Http::serveFile(response, "files" + request.resource());
    }
    else
    {
        response.send(Pistache::Http::Code::Not_Found, "Isso non ecziste!");
    }
}