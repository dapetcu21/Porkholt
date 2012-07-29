require("init_common")

lyr = { scale = 0.2 }
scl = 1/lyr.scale
for i=-2,3 do
	ci = math.random(5)
	layerAddImage(lyr,"/cloud"..ci.."",i*5*scl+(math.random()%2-1)*scl,3*scl+(math.random()%2-1)*scl,362/480*3*scl,156/480*3*scl)
end
addLayer(lyr)

--hook taranesc
Initializers._PHLPlayer = Initializers.PHLPlayer
function Initializers.PHLPlayer(obj)
	Initializers._PHLPlayer(obj)
	for i=0,obj.images.n-1 do
		if (obj.images[i].filename == "/ball") then
			obj.images[i].filename = "/ball_green"
		end	
	end
end

require("lvl_designer")
