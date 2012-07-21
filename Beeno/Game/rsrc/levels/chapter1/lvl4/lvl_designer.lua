--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

local obj0 = objectWithClass("PHLObject")
obj0.levelDes = true
obj0.rotation = 0.000000
obj0.pos = point(1.614151,-0.203711)
objectAddImage(obj,[[temple]],2.479374,0.700547,8.000000,6.042969)
addObject(obj0)

local player = objectWithClass("PHLPlayer")
player.levelDes = true
player.maximumForce = 40.000000
player.scripting = [[player]]
player.maxVelocityX = 2.500000
player.rotation = 0.000000
player.barHidden = true
player.trail = false
player.pos = point(2.357300,0.756473)
addObject(player)

local obj1 = objectWithClass("PHLAuxLayer")
obj1.levelDes = true
obj1.name = 20.000000
obj1.rotation = 0.000000
obj1.pos = point(0.000000,0.000000)
addObject(obj1)

local camera = objectWithClass("PHLCamera")
camera.levelDes = true
camera.camH = 4.000000
camera.rotation = 0.000000
camera.scripting = [[camera]]
camera.pos = point(-0.640588,1.626719)
addObject(camera)

local obj2 = objectWithClass("PHLObject")
obj2.levelDes = true
obj2.rotation = 0.000000
obj2.pos = point(12.610154,0.740000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj2)

local obj3 = objectWithClass("PHLObject")
obj3.levelDes = true
obj3.rotation = 0.000000
obj3.pos = point(12.430154,1.230000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj3)

local obj4 = objectWithClass("PHLObject")
obj4.levelDes = true
obj4.rotation = 0.000000
obj4.pos = point(12.650154,1.720000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj4)

local obj5 = objectWithClass("PHLObject")
obj5.levelDes = true
obj5.rotation = 0.000000
obj5.pos = point(12.870154,2.210000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj5)

local obj6 = objectWithClass("PHLObject")
obj6.levelDes = true
obj6.rotation = 0.000000
obj6.pos = point(13.410154,1.230000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj6)

local obj7 = objectWithClass("PHLObject")
obj7.levelDes = true
obj7.rotation = 0.000000
obj7.pos = point(13.140154,1.720000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj7)

local obj8 = objectWithClass("PHLObject")
obj8.levelDes = true
obj8.rotation = 0.000000
obj8.pos = point(12.120154,0.740000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj8)

local obj9 = objectWithClass("PHLObject")
obj9.levelDes = true
obj9.rotation = 0.000000
obj9.pos = point(13.590154,0.740000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj9)

local obj10 = objectWithClass("PHLObject")
obj10.levelDes = true
obj10.rotation = 0.000000
obj10.pos = point(13.100154,0.740000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj10)

local obj11 = objectWithClass("PHLObject")
obj11.levelDes = true
obj11.rotation = 0.000000
obj11.pos = point(12.920154,1.230000)
objectAddImage(obj,[[/box]],-0.250000,-0.250000,0.500000,0.500000)
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(obj11)

local obj12 = objectWithClass("PHLObject")
obj12.levelDes = true
obj12.rotation = 0.000000
obj12.pos = point(0.000000,0.260000)
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
objectAddBox(obj,0.000000,-0.250000,20.000000,0.500000)
addObject(obj12)

local obj13 = objectWithClass("PHLObject")
obj13.levelDes = true
obj13.rotation = 0.000000
obj13.pos = point(0.000000,0.000000)
objectAddBox(obj,-0.100000,0.000000,0.100000,10.000000)
addObject(obj13)

local obj14 = objectWithClass("PHLObject")
obj14.levelDes = true
obj14.rotation = 0.000000
obj14.pos = point(13.835214,0.000000)
objectAddBox(obj,0.000000,0.000000,0.100000,10.000000)
addObject(obj14)

local master = objectWithClass("PHLNPC")
master.levelDes = true
master.faceFlipping = true
master.questPoint = point(0.150000,0.600000)
master.scripting = [[master]]
master.hovers = true
master.rotation = 0.000000
master.staticFace = true
master.automaticFlipping = true
master.overHead = point(0.150000,0.600000)
master.pos = point(3.919019,0.748385)
objectAddImage(obj,[[/master_body]],-0.455117,-0.125156,1.137695,0.860157,{ class = [[PHTrailImageView]], tag = 20 })
objectAddImage(obj,[[/master_face]],-0.451875,-0.131094,1.137695,0.860157,{ tag = 21 })
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(master)

