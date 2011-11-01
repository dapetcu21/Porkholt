--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

local obj0 = objectWithClass("PHLPlatform")
obj0.levelDes = true
obj0.rotation = 0.000000
obj0.pos = point(14.000000,3.000000)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj0)

local obj1 = objectWithClass("PHLAuxLayer")
obj1.levelDes = true
obj1.rotation = 0.000000
obj1.name = 20.000000
obj1.pos = point(0.000000,0.000000)
addObject(obj1)

local wrecker = objectWithClass("PHLObject")
wrecker.levelDes = true
wrecker.physics.dynamic = true
wrecker.rotation = 0.000000
wrecker.scripting = [[wrecker]]
wrecker.pos = point(17.000000,1.276953)
objectAddImage(obj,[[/wrecking_ball.png]],-0.700000,-0.576953,1.400000,2.800000)
objectAddCircle(obj,0.576953)
addObject(wrecker)

local bezierCurve0 = {
	points = { n=7,
		[0] = { point = point(0.000000,0.000000), tag = 1 },
		[1] = { point = point(0.000000,-1.000000) },
		[2] = { point = point(0.500000,1.000000) },
		[3] = { point = point(1.000000,1.000000) },
		[4] = { point = point(1.000000,0.000000), tag = 2 },
		[5] = { point = point(1.000000,1.000000), tag = 3 },
		[6] = { point = point(0.000000,1.000000), tag = 4 }},
	curves = { n=1,
		[0] = range(0,5)}
	}
local obj2 = objectWithClass("PHLPlatform")
obj2.levelDes = true
obj2.rotation = 0.000000
obj2.pos = point(17.000000,3.500000)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000,{ rotation = 180.000000, bezierPath = bezierCurve0 })
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-1.000000,-0.150000,2.000000,0.300000), rotation = 180.000000 })
addObject(obj2)

local obj3 = objectWithClass("PHLPlatform")
obj3.levelDes = true
obj3.rotation = 0.000000
obj3.pos = point(14.479570,4.295625)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000,{ rotation = 180.000000, bezierPath = bezierCurve0 })
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-1.000000,-0.150000,2.000000,0.300000), rotation = 180.000000 })
addObject(obj3)

local camera = objectWithClass("PHLCamera")
camera.levelDes = true
camera.camH = 4.500000
camera.rotation = 0.000000
camera.scripting = [[camera]]
camera.pos = point(3.000000,1.750000)
addObject(camera)

local obj4 = objectWithClass("PHLObject")
obj4.levelDes = true
obj4.rotation = 0.000000
obj4.pos = point(0.000000,0.000000)
objectAddBox(obj,-0.100000,0.000000,0.100000,10.000000)
addObject(obj4)

local obj5 = objectWithClass("PHLObject")
obj5.levelDes = true
obj5.rotation = 0.000000
obj5.pos = point(20.000000,0.000000)
objectAddBox(obj,0.000000,0.000000,0.100000,10.000000)
addObject(obj5)

local player = objectWithClass("PHLPlayer")
player.levelDes = true
player.scripting = [[player]]
player.rotation = 0.000000
player.pos = point(1.500000,2.750000)
addObject(player)

local obj6 = objectWithClass("PHLObject")
obj6.levelDes = true
obj6.rotation = 0.000000
obj6.pos = point(0.000000,0.250000)
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
addObject(obj6)

local obj7 = objectWithClass("PHLObject")
obj7.levelDes = true
obj7.rotation = 0.000000
obj7.pos = point(0.000000,0.000000)
addObject(obj7)

local sensor = objectWithClass("PHLSensor")
sensor.levelDes = true
sensor.rotation = 0.000000
sensor.scripting = [[sensor]]
sensor.pos = point(13.640000,0.930000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(sensor)

local obj8 = objectWithClass("PHLObject")
obj8.levelDes = true
obj8.rotation = 0.000000
obj8.pos = point(3.244219,1.082617)
objectAddImage(obj,[[/master_body.png]],6.957336,-0.372015,1.400000,1.058473)
objectAddImage(obj,[[/master_face.png]],6.957336,-0.372015,1.400000,1.058473)
addObject(obj8)

local obj9 = objectWithClass("PHLObject")
obj9.levelDes = true
obj9.rotation = 0.000000
obj9.pos = point(3.244219,1.082617)
objectAddImage(obj,[[bird_nostroke.png]],5.779306,2.348841,0.750000,0.726471)
addObject(obj9)

local joint

joint = jointWithClass("PHRevoluteJoint")
joint.levelDes = true
joint.body1 = obj2
joint.body2 = wrecker
addJoint(joint)
