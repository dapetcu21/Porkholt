--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

obj = objectWithClass("PHLObject");
obj.pos = point(1.614151,-0.203711);
objectAddImage(obj,[[temple.png]],2.479374,0.700547,8.000000,6.042969);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLPlayer");
obj.pos = point(2.357300,0.756473);
obj.barHidden = true;
obj.maxVelocityX = 2.500000;
obj.maximumForce = 40.000000;
obj.scripting = [[player]];
obj.trail = false;
obj.canBlink = true;
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLAuxLayer");
obj.pos = point(0.000000,0.000000);
obj.name = 20.000000;
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLCamera");
obj.pos = point(-0.640588,1.626719);
obj.camH = 4.000000;
obj.scripting = [[camera]];
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(12.610154,0.740000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(12.430154,1.230000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(12.650154,1.720000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(12.870154,2.210000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(13.410154,1.230000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(13.140154,1.720000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(12.120154,0.740000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(13.590154,0.740000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(13.100154,0.740000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(12.920154,1.230000);
objectAddImage(obj,[[/box.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(0.000000,0.260000);
objectAddImage(obj,[[/ground.png]],-8.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],-6.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],-8.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[/ground.png]],-4.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],-2.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],-4.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[/ground.png]],0.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],2.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],0.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[/ground.png]],4.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],6.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],4.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[/ground.png]],8.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],10.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],8.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[/ground.png]],12.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],14.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],12.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[/ground.png]],16.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],18.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],16.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[/ground.png]],20.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],22.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],20.000000,-0.150000,4.400000,0.400000);
objectAddImage(obj,[[/ground.png]],24.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/ground.png]],26.000000,-0.750000,2.200000,1.000000);
objectAddImage(obj,[[/grass.png]],24.000000,-0.150000,4.400000,0.400000);
objectAddBox(obj,0.000000,-0.250000,20.000000,0.500000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(0.000000,0.000000);
objectAddBox(obj,-0.100000,0.000000,0.100000,10.000000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(13.835214,0.000000);
objectAddBox(obj,0.000000,0.000000,0.100000,10.000000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLNPC");
obj.pos = point(3.919019,0.748385);
obj.automaticFlipping = true;
obj.faceFlipping = true;
obj.hovers = true;
obj.overHead = point(0.150000,0.600000);
obj.questPoint = point(0.150000,0.600000);
obj.scripting = [[master]];
obj.staticFace = true;
op = {};
op.class = [[PHTrailImageView]];
op.tag = 20.000000;
objectAddImage(obj,[[/master_body.png]],-0.455117,-0.125156,1.137695,0.860157,op);
op = {};
op.tag = 21.000000;
objectAddImage(obj,[[/master_face.png]],-0.451875,-0.131094,1.137695,0.860157,op);
op = {};
op.rotation = 0.000000;
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,op);
obj.levelDes = true;
addObject(obj);

