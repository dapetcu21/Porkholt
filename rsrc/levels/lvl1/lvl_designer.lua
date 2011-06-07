--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

obj = objectWithClass("PHObject");
obj.pos = point(1.038489,-0.200000);
obj.physics = {};
obj.physics.fixtures = {n=0;};
obj.rotation = 0.000000;
objectAddImage(obj,[[city_bg.png]],0.000000,0.000000,10.800000,3.675743);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLAuxLayer");
obj.pos = point(0.000000,0.000000);
obj.name = 20.000000;
obj.physics = {};
obj.physics.fixtures = {n=0;};
obj.rotation = 0.000000;
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLCamera");
obj.pos = point(3.000000,1.750000);
obj.camH = 4.000000;
obj.physics = {};
obj.physics.fixtures = {n=0;};
obj.rotation = 0.000000;
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(0.000000,0.250000);
obj.physics = {};
obj.physics.fixtures = {n=1;};
obj.physics.fixtures[0] = {};
obj.physics.fixtures[0].box = rect(0.000000,-0.250000,20.000000,0.500000);
obj.physics.fixtures[0].shape = [[box]];
obj.rotation = 0.000000;
objectAddImage(obj,[[ground.png]],-8.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],-6.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],-8.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[ground.png]],-4.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],-2.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],-4.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[ground.png]],0.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],2.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],0.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[ground.png]],4.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],6.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],4.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[ground.png]],8.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],10.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],8.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[ground.png]],12.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],14.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],12.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[ground.png]],16.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],18.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],16.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[ground.png]],20.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],22.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],20.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[ground.png]],24.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[ground.png]],26.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[grass.png]],24.000000,-0.150000,4.400000,0.400000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(0.000000,0.000000);
obj.physics = {};
obj.physics.fixtures = {n=1;};
obj.physics.fixtures[0] = {};
obj.physics.fixtures[0].box = rect(-0.100000,0.000000,0.100000,10.000000);
obj.physics.fixtures[0].shape = [[box]];
obj.rotation = 0.000000;
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(20.000000,0.000000);
obj.physics = {};
obj.physics.fixtures = {n=1;};
obj.physics.fixtures[0] = {};
obj.physics.fixtures[0].box = rect(0.000000,0.000000,0.100000,10.000000);
obj.physics.fixtures[0].shape = [[box]];
obj.rotation = 0.000000;
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHObject");
obj.pos = point(3.350000,0.730000);
obj.physics = {};
obj.physics.fixtures = {n=0;};
obj.rotation = 0.000000;
objectAddImage(obj,[[mayor.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddImage(obj,[[hat.png]],-0.200000,0.200000,0.200000,0.200000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLPlayer");
obj.pos = point(1.500000,2.750000);
obj.rotation = 0.000000;
obj.levelDes = true;
addObject(obj);

