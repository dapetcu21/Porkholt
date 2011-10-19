--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

local obj

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(14.000000,3.000000)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLAuxLayer")
obj.levelDes = true
obj.rotation = 0.000000
obj.name = 20.000000
obj.pos = point(0.000000,0.000000)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.physics.dynamic = true
obj.rotation = 0.000000
obj.scripting = [[wrecker]]
obj.pos = point(17.000000,1.276953)
objectAddImage(obj,[[/wrecking_ball.png]],-0.700000,-0.576953,1.400000,2.800000)
objectAddCircle(obj,0.576953)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(17.000000,3.500000)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,1.000000,1.00000,{ bezierPath = { 
	points = { 
		n = 9,
		[0]={point = point(0.1,0), tag = 4},
		[1]={point = point(1,0), tag = 3}, 
		[2]={point = point(1,1), tag = 2}, 
		[3]={point = point(0.2,1), tag = 1},
		[4]={point = point(0.3,0.7)},
		[5]={point = point(-1,0.6)},
		[6]={point = point(0.4,0.5)},
		[7]={point = point(-1,0.4)},
		[8]={point = point(0.5,0.3)}
	},
	curves = {
		n = 0,
		[0]=range(0,4)
	}}})
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLCamera")
obj.levelDes = true
obj.camH = 4.500000
obj.rotation = 0.000000
obj.scripting = [[camera]]
obj.pos = point(3.000000,1.750000)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(0.000000,0.000000)
objectAddBox(obj,-0.100000,0.000000,0.100000,10.000000)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(20.000000,0.000000)
objectAddBox(obj,0.000000,0.000000,0.100000,10.000000)
addObject(obj)

obj = objectWithClass("PHLPlayer")
obj.levelDes = true
obj.scripting = [[player]]
obj.rotation = 0.000000
obj.pos = point(1.500000,2.750000)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(0.000000,0.250000)
objectAddImage(obj,[[/ground.png]],-8.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],-6.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],-8.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],-4.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],-2.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],-4.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],0.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],2.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],0.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],4.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],6.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],4.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],8.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],10.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],8.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],12.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],14.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],12.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],16.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],18.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],16.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],20.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],22.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],20.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],24.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],26.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],24.000000,-0.150000,4.400000,0.400000)
objectAddBox(obj,0.000000,-0.250000,20.000000,0.500000)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(0.000000,0.000000)
addObject(obj)

obj = objectWithClass("PHLSensor")
obj.levelDes = true
obj.rotation = 0.000000
obj.scripting = [[sensor]]
obj.pos = point(13.640000,0.930000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(3.244219,1.082617)
objectAddImage(obj,[[/master_body.png]],6.957336,-0.372015,1.400000,1.058473)
objectAddImage(obj,[[/master_face.png]],6.957336,-0.372015,1.400000,1.058473)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(3.244219,1.082617)
objectAddImage(obj,[[bird_nostroke.png]],5.779306,2.348841,0.750000,0.726471)
addObject(obj)


