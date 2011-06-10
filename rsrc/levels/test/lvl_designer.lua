--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

obj = objectWithClass("PHObject");
obj.pos = point(3.460000,2.650000);
obj.SorinSucks = true;
obj.SorinSucksAmount = 12.000000;
obj.array = {n=1;};
obj.array[0] = 12.000000;
obj.rotation = 334.174236;
obj.untitled = rect(12.000000,123.000000,12.000000,123.000000);
op = {};
op.rotation = 38.049725;
objectAddImage(obj,[[platform.png]],-0.480000,-0.730000,2.000000,0.300000,op);
op = {};
op.rotation = 0.000000;
objectAddImage(obj,[[platform.png]],-1.000000,0.150000,2.000000,0.300000,op);
op = {};
op.rotation = 38.049725;
objectAddBox(obj,-0.480000,-0.730000,2.000000,0.300000,op);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHObject");
obj.pos = point(3.350000,0.730000);
objectAddImage(obj,[[mayor.png]],-0.250000,-0.250000,0.500000,0.500000);
objectAddImage(obj,[[hat.png]],-0.200000,0.200000,0.200000,0.200000);
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHObject");
obj.pos = point(0.000000,0.000000);
obj.untitled = {};
obj.untitled.untitled = [[]];
obj.untitled_ = [[]];
obj.levelDes = true;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(0.540000,1.040000);
op = {};
op.pos = point(1.110000,-0.050000);
objectAddCircle(obj,0.500000,op);
obj.levelDes = true;
addObject(obj);

