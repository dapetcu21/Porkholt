require("init_common");

lyr = { scale = 0.3; }
for i=-2,3 do
	layerAddImage(lyr,"hills.png",i*6/0.3,0,6.06/0.3,6/5*2/0.3);
end
addLayer(lyr);

lyr = { scale = 0.2; }
scl = 1/lyr.scale;
for i=-2,3 do
	ci = math.random(5);
	layerAddImage(lyr,"cloud"..ci..".png",i*5*scl+(math.random()%2-1)*scl,3*scl+(math.random()%2-1)*scl,362/480*3*scl,156/480*3*scl);
end
addLayer(lyr);

--podu
br1,br2 = createBridge(3,point(8+0.15,2.1),point(10-0.15,2.6),7,0.01,
function ()
	local obj = objectWithClass("PHLObject");
	obj.physics.dynamic = true;
	objectAddCircle(obj,0.15,{ friction = 0.3; density = 1 });
	objectAddImage(obj,"wood_log.png",-0.15,-0.15,0.3,0.3);
	return addObject(obj);
end
);
boxW = ((792-29-12)/792)*2;
posBox = (29/792-0.5)*2;


--platforma 0
obj = objectWithClass("PHLPlatform")
obj.pos = point(7,2);
objectAddBox(obj,posBox,-0.15,boxW,0.3)
objectAddImage(obj,"platform.png",-1,-0.15,2,0.3)
platforma0=addObject(obj);

--platforma0->pod
joint = jointWithClass("PHDistanceJoint");
joint.body1 = platforma0;
joint.body2 = br1;
joint.anchor1 = point(1.0,0.1);
joint.anchor2 = point(-0.15,0);
joint.collideConnected = false;
addJoint(joint);

--platforma 1
obj = objectWithClass("PHLPlatform");
obj.pos = point(11,2.5);
objectAddBox(obj,posBox,-0.15,boxW,0.3)
objectAddImage(obj,"platform.png",-1,-0.15,2,0.3)
platforma1 = addObject(obj);

--platforma1->pod
joint = jointWithClass("PHDistanceJoint");
joint.body1 = platforma1;
joint.body2 = br2;
joint.anchor1 = point(-1.0,0.1);
joint.anchor2 = point(0.15,0);
joint.collideConnected = false;
addJoint(joint);

--platforma 2
obj = objectWithClass("PHLPlatform");
obj.pos = point(14,3);
objectAddBox(obj,posBox,-0.15,boxW,0.3)
objectAddImage(obj,"platform.png",-1,-0.15,2,0.3);
addObject(obj);

--platforma 3
obj = objectWithClass("PHLPlatform");
obj.pos = point(17,3.5);
objectAddBox(obj,posBox,-0.15,boxW,0.3)
objectAddImage(obj,"platform.png",-1,-0.15,2,0.3);
platforma3=obj;

--trail
obj=objectWithClass("PHLAuxLayer");
obj.pos = point(0,0);
obj.name = 20;
addObject(obj);

--wrecking ball
obj = objectWithClass("PHLObject");
ballr = 211/512;
scale = 1.4;
obj.pos = point(17,3.5-(2-ballr)*scale);
obj.physics.dynamic = true;
objectAddCircle(obj,ballr*scale);
objectAddImage(obj,"wrecking_ball.png",-0.5*scale,-ballr*scale,1*scale,2*scale);
bilos=obj;

addObject(bilos);
addObject(platforma3);

--jah bless teh joint
joint = jointWithClass("PHRevoluteJoint");
joint.body1 = platforma3;
joint.body2 = bilos;
joint.anchor = point(17,3.5);
joint.worldCoordinates = true;
addJoint(joint);

--camera
obj = objectWithClass("PHLCamera");
obj.pos = point(3,1.75);
obj.camH = 4;
addObject(obj);

obj = objectWithClass("PHLObject");
obj.pos = point(0,0);
objectAddBox(obj,-0.1,0,0.1,10);
addObject(obj);


obj = objectWithClass("PHLObject");
obj.pos = point(20,0);
objectAddBox(obj,0,0,0.1,10);
addObject(obj);

obj = objectWithClass("PHLPlayer");
obj.pos = point(1.5,2.75);
addObject(obj);

--pamant
obj = objectWithClass("PHLObject")
obj.pos = point(0,0.25);
for i=-2,6 do
	objectAddImage(obj,"ground.png",4*i,-0.75,2*1.1,1);
	objectAddImage(obj,"ground.png",4*i+2,-0.75,2*1.1,1);
	objectAddImage(obj,"grass.png",4*i,-0.15,4*1.1,0.4);
end
objectAddBox(obj,0,-0.25,20,0.5)
addObject(obj);

require("lvl_designer");