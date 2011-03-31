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

obj = objectWithClass("PHLPlayer");
obj.posX = 1.5;
obj.posY = 2.75;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.posX = 4;
obj.posY = 2;
obj.rotation = 30;
obj.physics.dynamic = 1;
objectAddBox(obj,-0.5,-0.5,1,1,{ friction = 0.3; density = 0.1 });
objectAddImage(obj,"box.png",-0.5,-0.5,1,1);
addObject(obj);

obj = objectWithClass("PHLObject");
obj.posX = 8;
obj.posY = 1.5;
objectAddBox(obj,-2,-0.2,4,0.4);
objectAddImage(obj,"platform.png",-2,-0.2,4,0.4);
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

obj = objectWithClass("PHLObject");
obj.posX = 20;
obj.posY = 0;
objectAddBox(obj,0,0,0.1,10);
addObject(obj);