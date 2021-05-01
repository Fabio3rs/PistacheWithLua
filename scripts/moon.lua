function rotaTeste (url, ptr)
	view("mainView")
end

function novoteste (url, ptr)
	view("otherview")
end

routeRegister("/", rotaTeste)
routeRegister("/test/:name", novoteste)

