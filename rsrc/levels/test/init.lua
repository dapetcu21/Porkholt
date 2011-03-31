lyr = { scale = 1.0; }
layerAddImage(lyr,"lyr2.png",0,0,3.5/2*3,3.5);
addLayer(lyr);

lyr = { scale = 0.7; }
layerAddImage(lyr,"lyr3.png",0,0,3.5/2*3,3.5);
addLayer(lyr);

lyr = { scale = 0.3; }
layerAddImage(lyr,"lyr1.png",0,0,3.5/2*3,3.5);
addLayer(lyr);

obj = objectWithClass("PHLObject")
obj.posX = 3;
obj.posY = 0.25;
for i=-2,3 do
	objectAddImage(obj,"ground.png",4*i,-0.85,2*1.1,1*1.1);
	objectAddImage(obj,"ground.png",4*i+2,-0.85,2*1.1,1*1.1);
	objectAddImage(obj,"grass.png",4*i,-0.15,4*1.1,0.4);
end
objectAddBox(obj,-3,-0.25,20,0.5)
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
obj.posX = 10;
obj.posY = 0;
objectAddBox(obj,0,0,0.1,10);
addObject(obj);