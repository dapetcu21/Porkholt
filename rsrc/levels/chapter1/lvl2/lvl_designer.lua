--This file was generated with the Porkholt Level Designer
--Do not modify this file. If you do, please note that this
--file is reset every time you use the Level Designer

local obj

obj = objectWithClass("PHLSign")
obj.levelDes = true
obj.rotation = 0.000000
obj.text = [[You can jump using long, ample swipes. Don't flick!|Try bouncing by swiping down mid-air then up again after hitting the ground to jump way higher|Bouncing is really important. Try to master it]]
obj.scripting = [[sign1]]
obj.pos = point(4.758593,0.799492)
addObject(obj)

obj = objectWithClass("PHLSign")
obj.levelDes = true
obj.rotation = 0.000000
obj.text = [[You've just picked up a shield.|Shields protect you against enemies like that jolly looking bull.|Be carefull, though, once you are hit, the shield disappears.]]
obj.scripting = [[sign2]]
obj.pos = point(7.595820,2.256524)
addObject(obj)

obj = objectWithClass("PHLSign")
obj.levelDes = true
obj.rotation = 0.000000
obj.text = [[Wait! Where are you going? Do you know how to dash?|Dashing is simple. Swipe your finger across the screen horizontally. Exactly like jumping. Again, don't flick|You will find dashing useful in defeating the next bull by yourself.|Also, try jumping on these platforms to the top. You will find something nice.]]
obj.scripting = [[sign3]]
obj.pos = point(17.076015,0.827540)
addObject(obj)

obj = objectWithClass("PHLSign")
obj.levelDes = true
obj.rotation = 0.000000
obj.text = [[Now, this is a health potion. As its name implies, it replenishes your health in case you get hurt.]]
obj.scripting = [[hpsign]]
obj.pos = point(22.033515,7.641876)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.physics.dynamic = false
obj.flipped = true
obj.rotation = 0.000000
obj.pos = point(46.968785,1.004641)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddBox(obj,-0.135040,-0.680013,0.592884,0.500000,{ rotation = 29.752959, density = 0.300000 })
objectAddCircle(obj,0.272969)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(0.003799,0.257536)
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
addObject(obj)

obj = objectWithClass("PHLSensor")
obj.levelDes = true
obj.rotation = 0.000000
obj.scripting = [[sensordash]]
obj.pos = point(17.915160,0.798660)
objectAddBox(obj,-0.233672,-0.331094,0.500000,5.126914)
addObject(obj)

obj = objectWithClass("PHLPit")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(51.054632,-0.091171)
objectAddBox(obj,-0.250000,-0.250000,10.000000,0.500000)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(143.977869,-0.091171)
objectAddBox(obj,-0.250000,-0.250000,10.000000,20.000000)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(57.884203,0.248892)
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
addObject(obj)

obj = objectWithClass("PHLGround")
obj.levelDes = true
obj.rotation = 350.000000
obj.pos = point(106.534548,-1.216537)
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
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(114.672210,0.224119)
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
addObject(obj)

obj = objectWithClass("PHLGround")
obj.levelDes = true
obj.rotation = 10.000000
obj.pos = point(90.156939,0.258417)
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
addObject(obj)

obj = objectWithClass("PHLGround")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(98.308529,-1.175492)
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
addObject(obj)

obj = objectWithClass("PHLCamera")
obj.levelDes = true
obj.camH = 4.000000
obj.rotation = 0.000000
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
obj.pos = point(0.000000,0.000000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(6.759571,1.805156)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(18.546811,1.770986)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(21.090599,2.643906)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(23.564278,3.768204)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(22.889437,7.156362)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(26.326217,4.790937)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(25.528543,6.275967)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(29.399795,5.664297)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddImage(obj,[[/platform.png]],0.888984,-0.148073,2.000000,0.300000)
objectAddImage(obj,[[/platform.png]],2.764295,-0.149795,2.000000,0.300000)
objectAddImage(obj,[[/platform.png]],4.645466,-0.150021,2.000000,0.300000)
objectAddBox(obj,-0.924027,-0.151407,7.535334,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(55.820019,1.725202)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(53.933975,1.724008)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLPlatform")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(54.796595,3.633602)
objectAddImage(obj,[[/platform.png]],-1.000000,-0.150000,2.000000,0.300000)
objectAddBox(obj,-0.926768,-0.150000,1.896465,0.300000)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.flipped = true
obj.pos = point(11.462495,0.820086)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.flipped = true
obj.pos = point(25.845773,0.822952)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.attackVelocity = 7.000000
obj.flipped = true
obj.pos = point(42.948468,0.829983)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.flipped = true
obj.pos = point(69.738017,0.825293)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.healthPoints = 3.000000
obj.cooldownDuration = 2.500000
obj.rotation = 0.000000
obj.attackVelocity = 6.000000
obj.flipped = true
obj.attackRange = 5.000000
obj.pos = point(102.286199,-0.450262)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.flipped = true
obj.pos = point(63.673884,0.824130)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.attackRange = 4.500000
obj.pos = point(33.283127,6.140639)
addObject(obj)

obj = objectWithClass("PHLLevelEnd")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(140.162274,0.631801)
objectAddBox(obj,-0.250000,-0.250000,0.502930,8.102695)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.physics.dynamic = false
obj.rotation = 0.000000
obj.pos = point(39.595316,1.003862)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddBox(obj,-0.135040,-0.680013,0.592884,0.500000,{ rotation = 29.752959, density = 0.300000 })
objectAddCircle(obj,0.272969)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.physics.dynamic = false
obj.flipped = true
obj.rotation = 0.000000
obj.pos = point(84.135968,1.002590)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddBox(obj,-0.135040,-0.680013,0.592884,0.500000,{ rotation = 29.752959, density = 0.300000 })
objectAddCircle(obj,0.272969)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.attackVelocity = 7.000000
obj.flipped = true
obj.pos = point(80.115651,0.827932)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.physics.dynamic = false
obj.rotation = 0.000000
obj.pos = point(76.762499,1.001810)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddBox(obj,-0.135040,-0.680013,0.592884,0.500000,{ rotation = 29.752959, density = 0.300000 })
objectAddCircle(obj,0.272969)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.flipped = true
obj.pos = point(118.135939,0.785518)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.physics.dynamic = false
obj.flipped = true
obj.rotation = 0.000000
obj.pos = point(132.533890,0.962815)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddBox(obj,-0.135040,-0.680013,0.592884,0.500000,{ rotation = 29.752959, density = 0.300000 })
objectAddCircle(obj,0.272969)
addObject(obj)

obj = objectWithClass("PHLBull")
obj.levelDes = true
obj.rotation = 0.000000
obj.attackVelocity = 7.000000
obj.flipped = true
obj.pos = point(128.513573,0.788157)
addObject(obj)

obj = objectWithClass("PHLObject")
obj.levelDes = true
obj.physics.dynamic = false
obj.rotation = 0.000000
obj.pos = point(125.160421,0.962035)
objectAddImage(obj,[[/rock.png]],-0.500000,-0.500000,1.334141,0.667070)
objectAddBox(obj,-0.135040,-0.680013,0.592884,0.500000,{ rotation = 29.752959, density = 0.300000 })
objectAddCircle(obj,0.272969)
addObject(obj)

obj = objectWithClass("PHLAuxLayer")
obj.levelDes = true
obj.name = 20.000000
obj.rotation = 0.000000
obj.pos = point(0.000000,0.000000)
addObject(obj)

obj = objectWithClass("PHLPlayer")
obj.levelDes = true
obj.maximumForce = 40.000000
obj.scripting = [[player]]
obj.maxVelocityX = 2.500000
obj.rotation = 0.000000
obj.barHidden = true
obj.trail = false
obj.pos = point(1.901328,0.871719)
addObject(obj)

obj = objectWithClass("PHLShieldPowerup")
obj.levelDes = true
obj.rotation = 0.000000
obj.scripting = [[shieldpup]]
obj.pos = point(6.642872,2.238838)
addObject(obj)

obj = objectWithClass("PHLHPPowerup")
obj.levelDes = true
obj.rotation = 0.000000
obj.scripting = [[hppotion]]
obj.pos = point(22.989504,7.593380)
addObject(obj)

obj = objectWithClass("PHLHPPowerup")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(54.751570,4.080325)
addObject(obj)

obj = objectWithClass("PHLHPPowerup")
obj.levelDes = true
obj.rotation = 0.000000
obj.pos = point(111.780477,1.671821)
addObject(obj)


