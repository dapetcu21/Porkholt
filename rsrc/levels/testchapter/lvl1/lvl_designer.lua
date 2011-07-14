--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

obj = objectWithClass("PHLObject");
obj.pos = point(0.000000,0.000000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLSensor");
obj.pos = point(13.640000,0.930000);
obj.scripting = [[sensor]];
op = {};
op.rotation = 0.000000;
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,op);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHObject");
obj.pos = point(3.350000,0.730000);
objectAddImage(obj,[[mayor.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddImage(obj,[[hat.png]],-0.200000,0.200000,0.200000,0.200000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHObject");
obj.pos = point(5.020469,0.706211);
objectAddImage(obj,[[bull_idle.png]],-0.235312,-0.176484,1.900000,0.839361);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHObject");
obj.pos = point(3.244219,1.082617);
objectAddImage(obj,[[master.png]],6.957336,-0.372015,1.400000,1.058473);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHObject");
obj.pos = point(3.244219,1.082617);
objectAddImage(obj,[[bird_nostroke.png]],5.779306,2.348841,0.750000,0.726471);
obj.levelDes = true;
addObject(obj);

