--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

local sign1 = objectWithClass("PHLSign")
sign1.levelDes = true
sign1.rotation = 0.000000
sign1.text = [[You can jump using long, ample swipes. Don't flick!|Try bouncing by swiping down mid-air then up again after hitting the ground to jump way higher|Bouncing is really important. Try to master it]]
sign1.scripting = [[sign1]]
sign1.pos = point(4.758593,0.799492)
addObject(sign1)

local sign2 = objectWithClass("PHLSign")
sign2.levelDes = true
sign2.rotation = 0.000000
sign2.text = [[You've just picked up a shield.|Shields protect you against enemies like that jolly looking bull.|Be carefull, though, once you are hit, the shield disappears.]]
sign2.scripting = [[sign2]]
sign2.pos = point(7.595820,2.256524)
addObject(sign2)

local sign3 = objectWithClass("PHLSign")
sign3.levelDes = true
sign3.rotation = 0.000000
sign3.text = [[Wait! Where are you going? Do you know how to dash?|Dashing is simple. Swipe your finger across the screen horizontally. Exactly like jumping. Again, don't flick|You will find dashing useful in defeating the next bull by yourself.|Also, try jumping on these platforms to the top. You will find something nice.]]
sign3.scripting = [[sign3]]
sign3.pos = point(17.076015,0.827540)
addObject(sign3)

local hpsign = objectWithClass("PHLSign")
hpsign.levelDes = true
hpsign.rotation = 0.000000
hpsign.text = [[Now, this is a health potion. As its name implies, it replenishes your health in case you get hurt.]]
hpsign.scripting = [[hpsign]]
hpsign.pos = point(22.033515,7.641876)
addObject(hpsign)

local bezierCurve0 = {
	points = { n=6,
		[0] = { point = point(0.100808,-0.000176), tag = 1 },
		[1] = { point = point(0.770528,0.011960), tag = 2 },
		[2] = { point = point(0.835461,0.326368) },
		[3] = { point = point(0.472204,0.486475) },
		[4] = { point = point(0.154033,1.033496), tag = 3 },
		[5] = { point = point(0.258017,0.619419) }},
	curves = { n=1,
		[0] = range(1,6)}
	}
local obj0 = objectWithClass("PHLObject")
obj0.levelDes = true
obj0.physics.dynamic = false
obj0.flipped = true
obj0.rotation = 0.000000
obj0.pos = point(46.968785,1.004641)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-0.500000,-0.500000,1.334141,0.667070) })
addObject(obj0)

local obj1 = objectWithClass("PHLObject")
obj1.levelDes = true
obj1.rotation = 0.000000
obj1.pos = point(0.003799,0.257536)
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
objectAddImage(obj,[[/ground.png]],28.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],30.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],28.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],32.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],34.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],32.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],36.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],38.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],36.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],40.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],42.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],40.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],44.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],46.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],44.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],48.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],50.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],48.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],49.998818,-1.743234,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],49.993505,-2.559015,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],47.999931,-1.748497,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],47.988857,-2.557882,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],59.882235,-2.554519,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],59.883812,-1.731456,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],57.884925,-1.736718,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],57.873851,-2.546104,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],63.865691,-2.550023,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],63.871004,-1.734242,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],61.872117,-1.739505,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],61.861043,-2.548890,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],67.862919,-2.545210,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],67.868232,-1.729429,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],65.869346,-1.734692,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],65.858271,-2.544077,2.200000,1.000000)
objectAddBox(obj,0.000000,-0.250000,52.400000,0.500000)
addObject(obj1)

local sensordash = objectWithClass("PHLSensor")
sensordash.levelDes = true
sensordash.rotation = 0.000000
sensordash.scripting = [[sensordash]]
sensordash.pos = point(17.915160,0.798660)
objectAddBox(obj,-0.233672,-0.331094,0.500000,5.126914)
addObject(sensordash)

local obj2 = objectWithClass("PHLPit")
obj2.levelDes = true
obj2.rotation = 0.000000
obj2.pos = point(51.054632,-0.091171)
objectAddBox(obj,-0.250000,-0.250000,10.000000,0.500000)
addObject(obj2)

local obj3 = objectWithClass("PHLObject")
obj3.levelDes = true
obj3.rotation = 0.000000
obj3.pos = point(143.977869,-0.091171)
objectAddBox(obj,-0.250000,-0.250000,10.000000,20.000000)
addObject(obj3)

local obj4 = objectWithClass("PHLObject")
obj4.levelDes = true
obj4.rotation = 0.000000
obj4.pos = point(57.884203,0.248892)
objectAddImage(obj,[[/ground.png]],0.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],2.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],-0.171712,-0.150000,4.571712,0.400000)
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
objectAddImage(obj,[[/grass.png]],24.000562,-0.144050,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],28.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],30.025642,-0.762184,2.344755,1.000000)
objectAddImage(obj,[[/grass.png]],27.996296,-0.071524,4.329002,0.322175)
objectAddImage(obj,[[/ground.png]],31.577763,-1.823134,2.200000,1.000000,{ rotation = 190.157201 })
objectAddImage(obj,[[/ground.png]],29.473505,-1.690790,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],33.545830,-2.162900,2.200000,1.000000,{ rotation = 189.461128 })
objectAddImage(obj,[[/ground.png]],35.506111,-2.503489,2.200000,1.000000,{ rotation = 190.000000 })
objectAddImage(obj,[[/ground.png]],27.457724,-1.701337,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],27.464626,-2.678584,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],27.464626,-2.678584,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],29.513124,-2.679850,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],31.553295,-2.702077,2.200000,1.000000,{ rotation = 2.920042 })
objectAddBox(obj,0.000000,-0.250000,32.400000,0.500000)
addObject(obj4)

local obj5 = objectWithClass("PHLGround")
obj5.levelDes = true
obj5.rotation = 350.000000
obj5.pos = point(106.534548,-1.216537)
objectAddImage(obj,[[/ground.png]],-0.285432,-0.835706,2.200000,1.000000,{ rotation = 8.724283 })
objectAddImage(obj,[[/ground.png]],6.483342,-1.732282,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],0.513285,-1.786189,2.157391,1.083895,{ rotation = 178.500000 })
objectAddImage(obj,[[/ground.png]],0.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],2.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],0.187648,-0.041195,4.065604,0.290597)
objectAddImage(obj,[[/ground.png]],4.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],6.000000,-0.750000,2.282630,1.000000)
objectAddImage(obj,[[/grass.png]],4.000000,-0.150000,4.324700,0.400000)
objectAddImage(obj,[[/ground.png]],2.488707,-1.729249,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],4.484259,-1.728107,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],0.497951,-2.692006,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],2.549778,-2.687701,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],4.651473,-2.664412,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],6.671308,-2.692758,2.200000,1.000000,{ texCoord = rect(0.000000,0.000000,0.950000,1.000000), rotation = 3.000000 })
objectAddBox(obj,0.000000,-0.250000,8.323494,0.500000)
addObject(obj5)

local obj6 = objectWithClass("PHLObject")
obj6.levelDes = true
obj6.rotation = 0.000000
obj6.pos = point(114.672210,0.224119)
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
objectAddImage(obj,[[/ground.png]],28.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],30.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],28.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],0.404178,-1.730357,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],2.400370,-1.728806,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],4.390451,-1.729629,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],0.228644,-2.620439,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],2.261279,-2.612270,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],4.388298,-2.621515,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],6.381609,-2.617779,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],6.388512,-1.733872,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],8.370868,-2.620122,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],8.377770,-1.736215,2.200000,1.000000,{ rotation = 180.000000 })
objectAddBox(obj,0.015062,-0.250000,32.384938,0.500798)
addObject(obj6)

local obj7 = objectWithClass("PHLGround")
obj7.levelDes = true
obj7.rotation = 10.000000
obj7.pos = point(90.156939,0.258417)
objectAddImage(obj,[[/ground.png]],7.999569,-0.749752,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],0.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],2.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],0.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],4.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],6.000515,-0.751763,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],4.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],5.488542,-1.731351,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],7.467517,-1.603011,2.200000,1.000000,{ rotation = 173.375459 })
objectAddBox(obj,0.000000,-0.250000,8.400000,0.500000)
addObject(obj7)

local obj8 = objectWithClass("PHLGround")
obj8.levelDes = true
obj8.rotation = 0.000000
obj8.pos = point(98.308529,-1.175492)
objectAddImage(obj,[[/ground.png]],0.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],2.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],0.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],4.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/ground.png]],6.000000,-0.750000,2.200000,1.000000)
objectAddImage(obj,[[/grass.png]],4.000000,-0.150000,4.400000,0.400000)
objectAddImage(obj,[[/ground.png]],0.987194,-1.693604,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],2.988203,-1.693288,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],4.987630,-1.702709,2.200000,1.000000,{ rotation = 180.000000 })
objectAddImage(obj,[[/ground.png]],6.968564,-1.708684,2.200000,1.000000,{ rotation = 180.000000 })
objectAddBox(obj,0.000000,-0.250000,8.400000,0.500000)
addObject(obj8)

local obj9 = objectWithClass("PHLCamera")
obj9.levelDes = true
obj9.camH = 4.000000
obj9.rotation = 0.000000
obj9.pos = point(3.000000,1.750000)
addObject(obj9)

local obj10 = objectWithClass("PHLObject")
obj10.levelDes = true
obj10.rotation = 0.000000
obj10.pos = point(0.000000,0.000000)
objectAddBox(obj,-0.100000,0.000000,0.100000,10.000000)
addObject(obj10)

local obj11 = objectWithClass("PHLObject")
obj11.levelDes = true
obj11.rotation = 0.000000
obj11.pos = point(0.000000,0.000000)
addObject(obj11)

local obj12 = objectWithClass("PHLPlatform")
obj12.levelDes = true
obj12.rotation = 0.000000
obj12.pos = point(6.759571,1.805156)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj12)

local obj13 = objectWithClass("PHLPlatform")
obj13.levelDes = true
obj13.rotation = 0.000000
obj13.pos = point(18.546811,1.770986)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj13)

local obj14 = objectWithClass("PHLPlatform")
obj14.levelDes = true
obj14.rotation = 0.000000
obj14.pos = point(21.090599,2.643906)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj14)

local obj15 = objectWithClass("PHLPlatform")
obj15.levelDes = true
obj15.rotation = 0.000000
obj15.pos = point(23.564278,3.768204)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj15)

local obj16 = objectWithClass("PHLPlatform")
obj16.levelDes = true
obj16.rotation = 0.000000
obj16.pos = point(22.889437,7.156362)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj16)

local obj17 = objectWithClass("PHLPlatform")
obj17.levelDes = true
obj17.rotation = 0.000000
obj17.pos = point(26.326217,4.790937)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj17)

local obj18 = objectWithClass("PHLPlatform")
obj18.levelDes = true
obj18.rotation = 0.000000
obj18.pos = point(25.528543,6.275967)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj18)

local obj19 = objectWithClass("PHLPlatform")
obj19.levelDes = true
obj19.rotation = 0.000000
obj19.pos = point(29.399795,5.664297)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddImage(obj,[[/platform.png]],0.888984,-0.148073,2.000000,0.300000)
objectAddImage(obj,[[/platform.png]],2.764295,-0.149795,2.000000,0.300000)
objectAddImage(obj,[[/platform.png]],4.645466,-0.150021,2.000000,0.300000)
objectAddBox(obj,-0.924027,-0.151407,7.535334,0.300000)
addObject(obj19)

local obj20 = objectWithClass("PHLPlatform")
obj20.levelDes = true
obj20.rotation = 0.000000
obj20.pos = point(55.820019,1.725202)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj20)

local obj21 = objectWithClass("PHLPlatform")
obj21.levelDes = true
obj21.rotation = 0.000000
obj21.pos = point(53.933975,1.724008)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj21)

local obj22 = objectWithClass("PHLPlatform")
obj22.levelDes = true
obj22.rotation = 0.000000
obj22.pos = point(54.796595,3.633602)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj22)

local obj23 = objectWithClass("PHLBull")
obj23.levelDes = true
obj23.rotation = 0.000000
obj23.flipped = true
obj23.pos = point(11.462495,0.820086)
addObject(obj23)

local obj24 = objectWithClass("PHLBull")
obj24.levelDes = true
obj24.rotation = 0.000000
obj24.flipped = true
obj24.pos = point(25.845773,0.822952)
addObject(obj24)

local obj25 = objectWithClass("PHLBull")
obj25.levelDes = true
obj25.rotation = 0.000000
obj25.attackVelocity = 7.000000
obj25.flipped = true
obj25.pos = point(42.948468,0.829983)
addObject(obj25)

local obj26 = objectWithClass("PHLBull")
obj26.levelDes = true
obj26.rotation = 0.000000
obj26.flipped = true
obj26.pos = point(69.738017,0.825293)
addObject(obj26)

local obj27 = objectWithClass("PHLBigBull")
obj27.levelDes = true
obj27.healthPoints = 3.000000
obj27.cooldownDuration = 2.500000
obj27.rotation = 0.000000
obj27.attackVelocity = 6.000000
obj27.flipped = true
obj27.attackRange = 5.000000
obj27.pos = point(102.286199,-0.450262)
addObject(obj27)

local obj28 = objectWithClass("PHLBull")
obj28.levelDes = true
obj28.rotation = 0.000000
obj28.flipped = true
obj28.pos = point(63.673884,0.824130)
addObject(obj28)

local obj29 = objectWithClass("PHLBull")
obj29.levelDes = true
obj29.rotation = 0.000000
obj29.attackRange = 4.500000
obj29.pos = point(33.283127,6.140639)
addObject(obj29)

local obj30 = objectWithClass("PHLLevelEnd")
obj30.levelDes = true
obj30.rotation = 0.000000
obj30.pos = point(140.162274,0.631801)
objectAddBox(obj,-0.250000,-0.250000,0.502930,8.102695)
addObject(obj30)

local obj31 = objectWithClass("PHLObject")
obj31.levelDes = true
obj31.physics.dynamic = false
obj31.rotation = 0.000000
obj31.pos = point(39.595316,1.003862)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-0.500000,-0.500000,1.334141,0.667070) })
addObject(obj31)

local obj32 = objectWithClass("PHLObject")
obj32.levelDes = true
obj32.physics.dynamic = false
obj32.flipped = true
obj32.rotation = 0.000000
obj32.pos = point(84.135968,1.002590)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-0.500000,-0.500000,1.334141,0.667070) })
addObject(obj32)

local obj33 = objectWithClass("PHLBull")
obj33.levelDes = true
obj33.rotation = 0.000000
obj33.attackVelocity = 7.000000
obj33.flipped = true
obj33.pos = point(80.115651,0.827932)
addObject(obj33)

local obj34 = objectWithClass("PHLObject")
obj34.levelDes = true
obj34.physics.dynamic = false
obj34.rotation = 0.000000
obj34.pos = point(76.762499,1.001810)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-0.500000,-0.500000,1.334141,0.667070) })
addObject(obj34)

local obj35 = objectWithClass("PHLBull")
obj35.levelDes = true
obj35.rotation = 0.000000
obj35.flipped = true
obj35.pos = point(118.135939,0.785518)
addObject(obj35)

local obj36 = objectWithClass("PHLObject")
obj36.levelDes = true
obj36.physics.dynamic = false
obj36.flipped = true
obj36.rotation = 0.000000
obj36.pos = point(132.533890,0.962815)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-0.500000,-0.500000,1.334141,0.667070) })
addObject(obj36)

local obj37 = objectWithClass("PHLBull")
obj37.levelDes = true
obj37.rotation = 0.000000
obj37.attackVelocity = 7.000000
obj37.flipped = true
obj37.pos = point(128.513573,0.788157)
addObject(obj37)

local obj38 = objectWithClass("PHLObject")
obj38.levelDes = true
obj38.physics.dynamic = false
obj38.rotation = 0.000000
obj38.pos = point(125.160421,0.962035)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddFreeform(obj,bezierCurve0,{ frame = rect(-0.500000,-0.500000,1.334141,0.667070) })
addObject(obj38)

local obj39 = objectWithClass("PHLAuxLayer")
obj39.levelDes = true
obj39.name = 20.000000
obj39.rotation = 0.000000
obj39.pos = point(0.000000,0.000000)
addObject(obj39)

local player = objectWithClass("PHLPlayer")
player.levelDes = true
player.maximumForce = 40.000000
player.scripting = [[player]]
player.maxVelocityX = 2.500000
player.rotation = 0.000000
player.barHidden = true
player.trail = false
player.pos = point(1.901328,0.871719)
addObject(player)

local shieldpup = objectWithClass("PHLShieldPowerup")
shieldpup.levelDes = true
shieldpup.rotation = 0.000000
shieldpup.scripting = [[shieldpup]]
shieldpup.pos = point(6.642872,2.238838)
addObject(shieldpup)

local hppotion = objectWithClass("PHLHPPowerup")
hppotion.levelDes = true
hppotion.rotation = 0.000000
hppotion.scripting = [[hppotion]]
hppotion.pos = point(22.989504,7.593380)
addObject(hppotion)

local obj40 = objectWithClass("PHLHPPowerup")
obj40.levelDes = true
obj40.rotation = 0.000000
obj40.pos = point(54.751570,4.080325)
addObject(obj40)

local obj41 = objectWithClass("PHLHPPowerup")
obj41.levelDes = true
obj41.rotation = 0.000000
obj41.pos = point(111.780477,1.671821)
addObject(obj41)

