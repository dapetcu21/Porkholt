require("init_common");

lyr = { scale = 0.3; }
for i=-2,3 do
	layerAddImage(lyr,"hills.png",i*6/0.3,0,6.06/0.3,6/5*2/0.3);
end
addLayer(lyr);

lyr = { scale = 0.2; }
scl = 1/lyr.scale;
for i=-2,3 do
	ci = math.random(5);
	layerAddImage(lyr,"cloud"..ci..".png",i*5*scl+(math.random()%2-1)*scl,3*scl+(math.random()%2-1)*scl,362/480*3*scl,156/480*3*scl);
end
addLayer(lyr);


--trail
obj=objectWithClass("PHLAuxLayer");
obj.pos = point(0,0);
obj.name = 20;
addObject(obj);


--camera
obj = objectWithClass("PHLCamera");
obj.pos = point(3,1.75);
obj.camH = 4;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(0,0);
objectAddBox(obj,-0.1,0,0.1,10);
addObject(obj);


obj = objectWithClass("PHLObject");
obj.pos = point(20,0);
objectAddBox(obj,0,0,0.1,10);
addObject(obj);

obj = objectWithClass("PHLPlayer");
obj.pos = point(1.5,2.75);
addObject(obj);

--pamant
obj = objectWithClass("PHLObject")
obj.pos = point(0,0.25);
for i=-2,6 do
	objectAddImage(obj,"ground.png",4*i,-0.75,2*1.1,1);
	objectAddImage(obj,"ground.png",4*i+2,-0.75,2*1.1,1);
	objectAddImage(obj,"grass.png",4*i,-0.15,4*1.1,0.4);
end
objectAddBox(obj,0,-0.25,20,0.5)
addObject(obj);

require("lvl_designer");