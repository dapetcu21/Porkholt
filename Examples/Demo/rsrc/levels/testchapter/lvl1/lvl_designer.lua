--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

local obj0 = objectWithClass("PHLPlatform")
obj0.levelDes = true
obj0.rotation = 0.000000
obj0.pos = point(14.000000,3.000000)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000,{ repeatX = 2.000000 })
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
wrecker.pos = point(17.395441,1.362755)
objectAddImage(obj,[[/wrecking_ball]],-0.700000,-0.576953,1.400000,2.800000)
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
obj2.pos = point(14.482460,4.280196)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000,{ rotation = 180.000000, bezierPath = bezierCurve0 })
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-1.000000,-0.150000,2.000000,0.300000), rotation = 180.000000 })
addObject(obj2)

local obj3 = objectWithClass("PHLPlatform")
obj3.levelDes = true
obj3.rotation = 0.000000
obj3.pos = point(17.440116,3.698164)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000,{ rotation = 180.000000, bezierPath = bezierCurve0 })
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
obj5.pos = point(25.681406,-0.385351)
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
objectAddImage(obj,[[/ground]],-8.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],-6.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],-8.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],-4.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],-2.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],-4.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],0.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],2.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],0.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],4.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],6.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],4.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],8.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],10.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],8.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],12.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],14.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],12.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],16.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],18.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],16.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],20.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],22.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],20.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],24.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground]],26.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass]],24.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground]],28.081095,-0.767266,2.200000,1.000000)
objectAddImage(obj,[[/ground]],30.089100,-0.769200,2.200000,1.000000)
objectAddImage(obj,[[/ground]],32.089375,-0.784356,2.200000,1.000000)
objectAddImage(obj,[[/ground]],36.223124,-0.794903,2.200000,1.000000)
objectAddImage(obj,[[/ground]],34.207890,-0.792286,2.200000,1.000000)
objectAddImage(obj,[[/ground]],28.081095,-0.767266,2.200000,1.000000)
objectAddImage(obj,[[/grass]],28.005428,-0.156289,4.400000,0.400000)
objectAddImage(obj,[[/grass]],32.328279,-0.139023,4.400000,0.400000)
objectAddBox(obj,0.000000,-0.390391,26.396214,0.640391)
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
sensor.pos = point(2.201172,0.768594)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(sensor)

local obj8 = objectWithClass("PHLBull")
obj8.levelDes = true
obj8.flipped = true
obj8.rotation = 0.000000
obj8.pos = point(14.061874,0.827773)
addObject(obj8)

local obj9 = objectWithClass("PHLObject")
obj9.levelDes = true
obj9.physics.dynamic = true
obj9.rotation = 0.000000
obj9.pos = point(5.767070,0.747422)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj9)

local obj10 = objectWithClass("PHLObject")
obj10.levelDes = true
obj10.physics.dynamic = true
obj10.rotation = 0.000000
obj10.pos = point(6.300899,0.755000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj10)

local obj11 = objectWithClass("PHLObject")
obj11.levelDes = true
obj11.physics.dynamic = true
obj11.rotation = 0.000000
obj11.pos = point(6.033750,1.274804)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj11)

local obj12 = objectWithClass("PHLObject")
obj12.levelDes = true
obj12.physics.dynamic = true
obj12.rotation = 0.000000
obj12.pos = point(6.717321,2.419225)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj12)

local obj13 = objectWithClass("PHLObject")
obj13.levelDes = true
obj13.physics.dynamic = true
obj13.rotation = 0.000000
obj13.pos = point(7.202758,2.932956)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj13)

local obj14 = objectWithClass("PHLObject")
obj14.levelDes = true
obj14.physics.dynamic = true
obj14.rotation = 0.000000
obj14.pos = point(7.245110,2.426739)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj14)

local obj15 = objectWithClass("PHLObject")
obj15.levelDes = true
obj15.physics.dynamic = true
obj15.rotation = 0.000000
obj15.pos = point(6.687928,2.940650)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj15)

local obj16 = objectWithClass("PHLObject")
obj16.levelDes = true
obj16.physics.dynamic = true
obj16.rotation = 0.000000
obj16.pos = point(6.671237,3.464145)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj16)

local obj17 = objectWithClass("PHLObject")
obj17.levelDes = true
obj17.physics.dynamic = true
obj17.rotation = 0.000000
obj17.pos = point(7.206823,3.437998)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000,{ density = 0.300000 })
addObject(obj17)

local joint

joint = jointWithClass("PHRevoluteJoint")
joint.levelDes = true
joint.body1 = obj3
joint.body2 = wrecker
addJoint(joint)
