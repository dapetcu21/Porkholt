--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

local obj0 = objectWithClass("PHLAuxLayer")
obj0.levelDes = true
obj0.name = 20.000000
obj0.rotation = 0.000000
obj0.pos = point(0.000000,0.000000)
addObject(obj0)

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

local camera = objectWithClass("PHLCamera")
camera.levelDes = true
camera.camH = 5.000000
camera.rotation = 0.000000
camera.scripting = [[camera]]
camera.pos = point(-0.640600,2.626700)
addObject(camera)

local obj1 = objectWithClass("PHLObject")
obj1.levelDes = true
obj1.rotation = 0.000000
obj1.pos = point(0.048743,0.243752)
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
objectAddBox(obj,0.000000,-0.250000,20.000000,0.500000)
addObject(obj1)

local obj2 = objectWithClass("PHLObject")
obj2.levelDes = true
obj2.rotation = 0.000000
obj2.pos = point(20.046698,0.242740)
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
objectAddImage(obj,[[/master_body]],-0.455117,-0.125156,1.137695,0.860157,{ class = [[PHTrailImageView]], tag = 20 })
objectAddImage(obj,[[/master_face]],-0.451875,-0.131094,1.137695,0.860157,{ tag = 21 })
objectAddBox(obj,-0.250000,-0.250000,0.500000,0.500000)
addObject(master)

local obj5 = objectWithClass("PHLPlatform")
obj5.levelDes = true
obj5.rotation = 0.000000
obj5.pos = point(16.553101,2.587161)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj5)

local obj6 = objectWithClass("PHLPlatform")
obj6.levelDes = true
obj6.rotation = 0.000000
obj6.pos = point(12.498662,2.232125)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj6)

local obj7 = objectWithClass("PHLPlatform")
obj7.levelDes = true
obj7.rotation = 0.000000
obj7.pos = point(19.205544,4.044578)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj7)

local obj8 = objectWithClass("PHLPlatform")
obj8.levelDes = true
obj8.rotation = 0.000000
obj8.pos = point(14.302263,9.661262)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj8)

local obj9 = objectWithClass("PHLPlatform")
obj9.levelDes = true
obj9.rotation = 0.000000
obj9.pos = point(21.307258,5.749947)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj9)

local obj10 = objectWithClass("PHLPlatform")
obj10.levelDes = true
obj10.rotation = 0.000000
obj10.pos = point(19.139647,8.198003)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj10)

local obj11 = objectWithClass("PHLPlatform")
obj11.levelDes = true
obj11.rotation = 0.000000
obj11.pos = point(12.551951,7.704253)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj11)

local obj12 = objectWithClass("PHLPlatform")
obj12.levelDes = true
obj12.rotation = 0.000000
obj12.pos = point(13.639536,4.433373)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj12)

local obj13 = objectWithClass("PHLPlatform")
obj13.levelDes = true
obj13.rotation = 0.000000
obj13.pos = point(17.048800,5.780984)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj13)

local obj14 = objectWithClass("PHLPlatform")
obj14.levelDes = true
obj14.rotation = 0.000000
obj14.pos = point(9.568938,9.769851)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj14)

local obj15 = objectWithClass("PHLPlatform")
obj15.levelDes = true
obj15.rotation = 0.000000
obj15.pos = point(10.562899,11.306721)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj15)

local obj16 = objectWithClass("PHLPlatform")
obj16.levelDes = true
obj16.rotation = 0.000000
obj16.pos = point(14.357693,13.588658)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj16)

local obj17 = objectWithClass("PHLPlatform")
obj17.levelDes = true
obj17.rotation = 0.000000
obj17.pos = point(25.999466,27.098008)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj17)

local obj18 = objectWithClass("PHLPlatform")
obj18.levelDes = true
obj18.rotation = 0.000000
obj18.pos = point(24.106519,27.097917)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj18)

local obj19 = objectWithClass("PHLPowerPowerup")
obj19.levelDes = true
obj19.flipped = true
obj19.rotation = 0.000000
obj19.pos = point(14.368932,10.288743)
addObject(obj19)

local obj20 = objectWithClass("PHLPlatform")
obj20.levelDes = true
obj20.rotation = 0.000000
obj20.pos = point(22.477751,14.752571)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj20)

local obj21 = objectWithClass("PHLPlatform")
obj21.levelDes = true
obj21.rotation = 0.000000
obj21.pos = point(19.318119,15.610649)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj21)

local obj22 = objectWithClass("PHLPlatform")
obj22.levelDes = true
obj22.rotation = 0.000000
obj22.pos = point(17.971817,11.731034)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj22)

local obj23 = objectWithClass("PHLPlatform")
obj23.levelDes = true
obj23.rotation = 0.000000
obj23.pos = point(20.420411,9.650731)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj23)

local obj24 = objectWithClass("PHLPlatform")
obj24.levelDes = true
obj24.rotation = 0.000000
obj24.pos = point(10.944718,5.885741)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj24)

local obj25 = objectWithClass("PHLPlatform")
obj25.levelDes = true
obj25.rotation = 0.000000
obj25.pos = point(9.609400,13.250878)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj25)

local obj26 = objectWithClass("PHLPlatform")
obj26.levelDes = true
obj26.rotation = 0.000000
obj26.pos = point(8.899467,19.703079)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj26)

local obj27 = objectWithClass("PHLPlatform")
obj27.levelDes = true
obj27.rotation = 0.000000
obj27.pos = point(12.899467,20.203079)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj27)

local obj28 = objectWithClass("PHLPlatform")
obj28.levelDes = true
obj28.rotation = 0.000000
obj28.pos = point(16.334250,14.949429)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj28)

local obj29 = objectWithClass("PHLPlatform")
obj29.levelDes = true
obj29.rotation = 0.000000
obj29.pos = point(12.279811,14.594393)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj29)

local obj30 = objectWithClass("PHLPlatform")
obj30.levelDes = true
obj30.rotation = 0.000000
obj30.pos = point(14.083412,22.023530)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj30)

local obj31 = objectWithClass("PHLPlatform")
obj31.levelDes = true
obj31.rotation = 0.000000
obj31.pos = point(21.088407,18.112215)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj31)

local obj32 = objectWithClass("PHLPlatform")
obj32.levelDes = true
obj32.rotation = 0.000000
obj32.pos = point(18.920796,20.560271)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj32)

local obj33 = objectWithClass("PHLPlatform")
obj33.levelDes = true
obj33.rotation = 0.000000
obj33.pos = point(13.420685,16.795641)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj33)

local obj34 = objectWithClass("PHLPlatform")
obj34.levelDes = true
obj34.rotation = 0.000000
obj34.pos = point(16.829949,18.143252)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj34)

local obj35 = objectWithClass("PHLPlatform")
obj35.levelDes = true
obj35.rotation = 0.000000
obj35.pos = point(9.350087,22.132119)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj35)

local obj36 = objectWithClass("PHLPlatform")
obj36.levelDes = true
obj36.rotation = 0.000000
obj36.pos = point(10.344048,23.668989)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj36)

local obj37 = objectWithClass("PHLPlatform")
obj37.levelDes = true
obj37.rotation = 0.000000
obj37.pos = point(14.138842,25.950926)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj37)

local obj38 = objectWithClass("PHLPowerPowerup")
obj38.levelDes = true
obj38.flipped = true
obj38.rotation = 0.000000
obj38.pos = point(14.150081,22.651011)
addObject(obj38)

local obj39 = objectWithClass("PHLPlatform")
obj39.levelDes = true
obj39.rotation = 0.000000
obj39.pos = point(22.259806,27.096881)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj39)

local obj40 = objectWithClass("PHLPlatform")
obj40.levelDes = true
obj40.rotation = 0.000000
obj40.pos = point(19.099268,27.972917)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj40)

local obj41 = objectWithClass("PHLPlatform")
obj41.levelDes = true
obj41.rotation = 0.000000
obj41.pos = point(17.752966,24.093302)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj41)

local obj42 = objectWithClass("PHLPlatform")
obj42.levelDes = true
obj42.rotation = 0.000000
obj42.pos = point(20.201560,22.012999)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj42)

local obj43 = objectWithClass("PHLPlatform")
obj43.levelDes = true
obj43.rotation = 0.000000
obj43.pos = point(10.725867,18.248009)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj43)

local obj44 = objectWithClass("PHLPlatform")
obj44.levelDes = true
obj44.rotation = 0.000000
obj44.pos = point(8.899467,19.703079)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj44)

local obj45 = objectWithClass("PHLPlatform")
obj45.levelDes = true
obj45.rotation = 0.000000
obj45.pos = point(12.899467,20.203079)
objectAddImage(obj,[[/platform]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj45)

local obj46 = objectWithClass("PHLBull")
obj46.levelDes = true
obj46.flipped = true
obj46.rotation = 0.000000
obj46.pos = point(26.468679,27.596209)
addObject(obj46)

local obj47 = objectWithClass("PHLLevelEnd")
obj47.levelDes = true
obj47.rotation = 0.000000
obj47.pos = point(26.402500,27.632391)
addObject(obj47)

