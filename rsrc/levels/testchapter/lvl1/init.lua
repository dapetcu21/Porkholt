require("init_common");

local ar = 1600.0/282.0;
lyr = { scale = 0.15; }
local width = 6
local rep = 10
local posy = 0.8
local swatch_height = 10
layerAddImage(lyr,"/hills_back.png",-width*3/lyr.scale,posy/lyr.scale,width*rep/lyr.scale,width/ar/lyr.scale, { repeatX = rep });
layerAddImage(lyr,"/hills_back_swatch.png",-width*3/lyr.scale,(posy-swatch_height)/lyr.scale,width*rep/lyr.scale,swatch_height/lyr.scale, { repeatX = rep });
ar = 1600.0/256.0
addLayer(lyr);
lyr = { scale = 0.4 }
posy = 0.75
layerAddImage(lyr,"/hills_middle.png",-width*3/lyr.scale,posy/lyr.scale,width*rep/lyr.scale,width/ar/lyr.scale, { repeatX = rep });
layerAddImage(lyr,"/hills_middle_swatch.png",-width*3/lyr.scale,(posy-swatch_height)/lyr.scale,width*rep/lyr.scale,swatch_height/lyr.scale, { repeatX = rep });
addLayer(lyr);
ar = 1600.0/203.0
lyr = { scale = 0.8 }
posy = 0.6
layerAddImage(lyr,"/hills_front.png",-width*3/lyr.scale,posy/lyr.scale,width*rep/lyr.scale,width/ar/lyr.scale, { repeatX = rep });
layerAddImage(lyr,"/hills_front_swatch.png",-width*3/lyr.scale,(posy-swatch_height)/lyr.scale,width*rep/lyr.scale,swatch_height/lyr.scale, { repeatX = rep });
addLayer(lyr);

lyr = { scale = 0.2; }
scl = 1/lyr.scale;
for i=-2,3 do
	ci = math.random(5);
	layerAddImage(lyr,"/cloud"..ci..".png",i*5*scl+(math.random()%2-1)*scl,3*scl+(math.random()%2-1)*scl,362/480*3*scl,156/480*3*scl);
end
addLayer(lyr);

--podu
br1,br2 = createBridge(3,point(8+0.15,2.1),point(10-0.15,2.6),7,0.01,
function ()
	local obj = objectWithClass("PHLObject");
	obj.physics.dynamic = true;
	objectAddCircle(obj,0.15,{ friction = 0.3; density = 1 });
	objectAddImage(obj,"/wood_log.png",-0.15,-0.15,0.3,0.3);
	return addObject(obj);
end
);
boxW = ((792-29-12)/792)*2;
posBox = (29/792-0.5)*2;

local bezierCurve_0 = {
	points = { n=7,
		[0] = { point = point(7.000000,2.000000), tag = 1 },
		[1] = { point = point(7.000000,1.000000) },
		[2] = { point = point(7.500000,3.000000) },
		[3] = { point = point(8.000000,3.000000) },
		[4] = { point = point(8.000000,2.000000), tag = 2 },
		[5] = { point = point(8.000000,3.000000), tag = 3 },
		[6] = { point = point(7.000000,3.000000), tag = 4 }},
	curves = { n=1,
		[0] = range(0,5)}
}

--local bezierCurve_0 = {
--	points = { n=2,
--		[0] = { point = point(8.000000,3.000000), tag = 3 },
--		[1] = { point = point(7.000000,3.000000), tag = 4 }}
--}

--platforma 0
obj = objectWithClass("PHLPlatform")
obj.pos = point(7,2)
obj.patrolPath = bezierCurve_0
obj.patrolSpeed = 1
obj.patrolInCircle = true

objectAddBox(obj,posBox,-0.15,boxW,0.3)
objectAddImage(obj,"/platform.png",-1,-0.15,2,0.3)
platforma0=addObject(obj);

--platforma0->pod
joint = jointWithClass("PHDistanceJoint");
joint.body1 = platforma0;
joint.body2 = br1;
joint.anchor1 = point(1.0,0.05);
joint.anchor2 = point(-0.15,0);
joint.collideConnected = false;
addJoint(joint);

--platforma 1
obj = objectWithClass("PHLPlatform");
obj.pos = point(11,2.5);
objectAddBox(obj,posBox,-0.15,boxW,0.3)
objectAddImage(obj,"/platform.png",-1,-0.15,2,0.3)
platforma1 = addObject(obj);

--platforma1->pod
joint = jointWithClass("PHDistanceJoint");
joint.body1 = platforma1;
joint.body2 = br2;
joint.anchor1 = point(-1.0,0.1);
joint.anchor2 = point(0.15,0);
joint.collideConnected = false;
addJoint(joint);

require("lvl_designer");

--fireworks
obj = objectWithClass("PHLObject")
obj.pos = point(4,2)
objectAddImage(obj,"/particle.png",0,0,1,1,{ class = "PHParticleView", particleAnimator = { 
	velocity = vector(2,2), 
	spreadAngle = 60, 
	velocityVariation = 2, 
	initialSize = point(0.3,0.3), 
	finalSize = point(0.7,0.7), 
	initialColor = rgba(1,1,1,1), 
	finalColor = rgba(1,1,1,0.5)
	} })
addObject(obj)