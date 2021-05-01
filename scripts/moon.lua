json = require "scripts/req/json"

function api_online (url, ptr)
	obj = {}
	obj["Status"] = "Ok"
	vp(json.encode(obj))
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

