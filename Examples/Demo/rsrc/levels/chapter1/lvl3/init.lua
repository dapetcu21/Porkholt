require("init_common")

lyr = { scale = 0.3 }
for i=-2,20 do
	layerAddImage(lyr,"/hills",i*6/0.3,-6/5*2/0.3,6.06/0.3,6/5*2/0.3*2)
end
addLayer(lyr)

lyr = { scale = 0.2 }
scl = 1/lyr.scale
for i=-2,3 do
	ci = math.random(5)
	layerAddImage(lyr,"/cloud"..ci.."",i*5*scl+(math.random()%2-1)*scl,3*scl+(math.random()%2-1)*scl,362/480*3*scl,156/480*3*scl)
end
addLayer(lyr)

require("lvl_designer")
