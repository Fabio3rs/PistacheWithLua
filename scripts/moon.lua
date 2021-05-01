json = require "scripts/req/json"

set_cookies = {}

function api_online (url, ptr)
	obj = {}
	obj["Status"] = "Ok"
	
	if _cookies["req"] then
		obj["ReqCookie"] = _cookies["req"]
	end
	vp(json.encode(obj))

	set_cookies["req"] = "a"
end

function rotaTeste (url, ptr)
	view("mainView")
end

function novoteste (url, ptr)
	view("otherview")
end

routeRegister("/api/status", api_online)
routeRegister("/", rotaTeste)
routeRegister("/test/:name", novoteste)

