require("init_common");

lyr = { scale = 0.3; }
for i=-2,3 do
	layerAddImage(lyr,"/hills.png",i*6/0.3,0,6.06/0.3,6/5*2/0.3);
end
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


--platforma 0
obj = objectWithClass("PHLPlatform")
obj.pos = point(7,2);
objectAddBox(obj,posBox,-0.15,boxW,0.3)
objectAddImage(obj,"/platform.png",-1,-0.15,2,0.3)
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