--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

local player = objectWithClass("PHLPlayer")
player.levelDes = true
player.maximumForce = 40.000000
player.scripting = [[player]]
player.maxVelocityX = 2.500000
player.barHidden = true
player.rotation = 0.000000
player.trail = false
player.pos = point(2.357300,0.756473)
addObject(player)

local obj0 = objectWithClass("PHLAuxLayer")
obj0.levelDes = true
obj0.name = 20.000000
obj0.rotation = 0.000000
obj0.pos = point(0.000000,0.000000)
addObject(obj0)

local camera = objectWithClass("PHLCamera")
camera.levelDes = true
camera.camH = 6.000000
camera.rotation = 0.000000
camera.scripting = [[camera]]
camera.pos = point(-0.640600,2.626700)
addObject(camera)

local obj1 = objectWithClass("PHLObject")
obj1.levelDes = true
obj1.rotation = 0.000000
obj1.pos = point(0.048743,0.243752)
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
objectAddBox(obj,0.000000,-0.250000,20.000000,0.500000)
addObject(obj1)

local obj2 = objectWithClass("PHLObject")
obj2.levelDes = true
obj2.rotation = 0.000000
obj2.pos = point(20.046698,0.242740)
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
objectAddBox(obj,0.000000,-0.250000,20.000000,0.500000)
addObject(obj2)

local obj3 = objectWithClass("PHLObject")
obj3.levelDes = true
obj3.rotation = 0.000000
obj3.pos = point(0.000000,0.000000)
objectAddBox(obj,-0.100000,0.000000,0.126400,30.000000)
addObject(obj3)

local obj4 = objectWithClass("PHLObject")
obj4.levelDes = true
obj4.rotation = 0.000000
obj4.pos = point(40.020567,-0.199930)
objectAddBox(obj,0.000000,0.000000,0.100000,30.000000)
addObject(obj4)

local master = objectWithClass("PHLNPC")
master.levelDes = true
master.faceFlipping = true
master.scripting = [[master]]
master.overHead = point(0.150000,0.600000)
master.hovers = true
master.rotation = 0.000000
master.staticFace = true
master.automaticFlipping = true
master.questPoint = point(0.150000,0.600000)
master.pos = point(3.918980,0.728385)
objectAddImage(obj,[[/master_body.png]],-0.455117,-0.125156,1.137695,0.860157,{ class = [[PHTrailImageView]], tag = 20 })
objectAddImage(obj,[[/master_face.png]],-0.451875,-0.131094,1.137695,0.860157,{ tag = 21 })
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(master)

local obj5 = objectWithClass("PHLPlatform")
obj5.levelDes = true
obj5.rotation = 0.000000
obj5.pos = point(12.498662,2.232125)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj5)

local obj6 = objectWithClass("PHLPlatform")
obj6.levelDes = true
obj6.rotation = 0.000000
obj6.pos = point(14.302263,9.661262)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj6)

local obj7 = objectWithClass("PHLPlatform")
obj7.levelDes = true
obj7.rotation = 0.000000
obj7.pos = point(13.639536,4.433373)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj7)

local obj8 = objectWithClass("PHLPlatform")
obj8.levelDes = true
obj8.rotation = 0.000000
obj8.pos = point(14.357693,13.588658)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj8)

local obj9 = objectWithClass("PHLPlatform")
obj9.levelDes = true
obj9.rotation = 0.000000
obj9.pos = point(13.900191,20.569681)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj9)

local obj10 = objectWithClass("PHLPowerPowerup")
obj10.levelDes = true
obj10.flipped = true
obj10.rotation = 0.000000
obj10.pos = point(13.724667,5.050109)
addObject(obj10)

local obj11 = objectWithClass("PHLPlatform")
obj11.levelDes = true
obj11.rotation = 0.000000
obj11.pos = point(14.033412,24.073530)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj11)

local obj12 = objectWithClass("PHLPlatform")
obj12.levelDes = true
obj12.rotation = 0.000000
obj12.pos = point(14.138842,25.950926)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj12)

local obj13 = objectWithClass("PHLPowerPowerup")
obj13.levelDes = true
obj13.flipped = true
obj13.rotation = 0.000000
obj13.pos = point(14.435138,14.503590)
addObject(obj13)

local obj14 = objectWithClass("PHLPlatform")
obj14.levelDes = true
obj14.rotation = 0.000000
obj14.pos = point(20.662857,32.607611)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj14)

local obj15 = objectWithClass("PHLPlatform")
obj15.levelDes = true
obj15.rotation = 0.000000
obj15.pos = point(17.100130,29.579722)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj15)

local obj16 = objectWithClass("PHLPlatform")
obj16.levelDes = true
obj16.rotation = 0.000000
obj16.pos = point(20.718287,36.535007)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj16)

local obj17 = objectWithClass("PHLPowerPowerup")
obj17.levelDes = true
obj17.flipped = true
obj17.rotation = 0.000000
obj17.pos = point(17.135261,30.446459)
addObject(obj17)

local obj18 = objectWithClass("PHLPowerPowerup")
obj18.levelDes = true
obj18.flipped = true
obj18.rotation = 0.000000
obj18.pos = point(20.885261,37.796459)
addObject(obj18)

local obj19 = objectWithClass("PHLPlatform")
obj19.levelDes = true
obj19.rotation = 0.000000
obj19.pos = point(18.294006,40.820074)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj19)

local obj20 = objectWithClass("PHLPlatform")
obj20.levelDes = true
obj20.rotation = 0.000000
obj20.pos = point(23.099436,48.897861)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj20)

local obj21 = objectWithClass("PHLPlatform")
obj21.levelDes = true
obj21.rotation = 0.000000
obj21.pos = point(16.952365,13.705478)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj21)

local obj22 = objectWithClass("PHLPlatform")
obj22.levelDes = true
obj22.rotation = 0.000000
obj22.pos = point(22.049436,54.497861)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj22)

local obj23 = objectWithClass("PHLPlatform")
obj23.levelDes = true
obj23.rotation = 0.000000
obj23.pos = point(17.949436,51.447861)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj23)

local obj24 = objectWithClass("PHLPowerPowerup")
obj24.levelDes = true
obj24.flipped = true
obj24.rotation = 0.000000
obj24.pos = point(17.795732,52.300525)
addObject(obj24)

local obj25 = objectWithClass("PHLPowerPowerup")
obj25.levelDes = true
obj25.flipped = true
obj25.rotation = 0.000000
obj25.pos = point(18.196709,42.054626)
addObject(obj25)

local obj26 = objectWithClass("PHLLevelEnd")
obj26.levelDes = true
obj26.rotation = 0.000000
obj26.pos = point(22.036125,55.111496)
addObject(obj26)

local sensordash = objectWithClass("PHLSensor")
sensordash.levelDes = true
sensordash.rotation = 0.000000
sensordash.scripting = [[sensordash]]
sensordash.pos = point(13.664887,14.224676)
objectAddBox(obj,-0.233700,-0.331100,2.000000,5.126900)
addObject(sensordash)

local sign4 = objectWithClass("PHLSign")
sign4.levelDes = true
sign4.text = [[When in doubt, go up!]]
sign4.rotation = 0.000000
sign4.scripting = [[sign4]]
sign4.pos = point(16.993789,14.307345)
addObject(sign4)

local obj27 = objectWithClass("PHLPlatform")
obj27.levelDes = true
obj27.rotation = 0.000000
obj27.pos = point(20.854318,40.855869)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj27)

local sensordash = objectWithClass("PHLSensor")
sensordash.levelDes = true
sensordash.rotation = 0.000000
sensordash.scripting = [[sensordash]]
sensordash.pos = point(17.573949,41.415067)
objectAddBox(obj,-0.233700,-0.331100,2.000000,5.126900)
addObject(sensordash)

local sign3 = objectWithClass("PHLSign")
sign3.levelDes = true
sign3.text = [[When in doubt, go up!]]
sign3.rotation = 0.000000
sign3.scripting = [[sign3]]
sign3.pos = point(20.942851,41.487736)
addObject(sign3)

