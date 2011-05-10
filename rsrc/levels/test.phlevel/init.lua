require("init_common");
-- aci bagi nebunia de obiecte!!
lyr = { scale = 0.3; }
for i=-2,3 do
	layerAddImage(lyr,"lyr1.png",i*6/0.3,0,6.06/0.3,6/5*2/0.3);
end
addLayer(lyr);

lyr = { scale = 0.2; }
scl = 1/lyr.scale;
for i=-2,3 do
	ci = math.random(5);
	layerAddImage(lyr,"cloud"..ci..".png",i*5*scl+(math.random()%2-1)*scl,3*scl+(math.random()%2-1)*scl,362/480*3*scl,156/480*3*scl);
end
addLayer(lyr);


for i=0,4 do
	for j=0,0 do
		obj = objectWithClass("PHLObject");
		obj.posX = 4+i;
		obj.posY = 2+j;
		obj.physics.dynamic = true;
		objectAddCircle(obj,0.3,{ friction = 0.3; density = 0.1 });
		objectAddImage(obj,"cartoonish_wood_texture_log.png",-0.3,-0.3,0.6,0.6);
		addObject(obj);
	end
end 

--obj = objectWithClass("PHLObject");
--obj.posX = 3;
--obj.posY = 1.5;
--obj.physics.dynamic = false;
--objectAddCircle(obj,0.5,{ friction = 0.3; density = 0.1 });
--objectAddImage(obj,"mofo.png",-0.5,-0.5,1,1);
--addObject(obj);


obj = objectWithClass("PHLObject");
obj.posX = 8;
obj.posY = 2.5;
objectAddBox(obj,-2,-0.2,4,0.4);
objectAddImage(obj,"platform.png",-2,-0.2,4,0.4,{alpha=0.5; tint=colorWithRGBA(1,0.05,0.05);});
addObject(obj);

obj = objectWithClass("PHLCamera");
obj.posX = 3;
obj.posY = 1.75;
obj.camH = 4;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.posX = 0;
obj.posY = 0;
objectAddBox(obj,-0.1,0,0.1,10);
addObject(obj);
--obj = objectWithClass("PHLObject");
--ma pis pe nebunia asta
--obj = objectWithClass("PHLObject");
--obj.posX = 


obj = objectWithClass("PHLObject");
obj.posX = 20;
obj.posY = 0;
objectAddBox(obj,0,0,0.1,10);
addObject(obj);

obj = objectWithClass("PHLPlayer");
obj.posX = 1.5;
obj.posY = 2.75;
addObject(obj);

obj = objectWithClass("PHLObject")
obj.posX = 0;
obj.posY = 0.25;
for i=-2,6 do
	objectAddImage(obj,"ground.png",4*i,-0.85,2*1.1,1*1.1);
	objectAddImage(obj,"ground.png",4*i+2,-0.85,2*1.1,1*1.1);
	objectAddImage(obj,"grass.png",4*i,-0.15,4*1.1,0.4);
end
objectAddBox(obj,0,-0.25,20,0.5)
addObject(obj);

require("lvl_designer");