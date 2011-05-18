objects = { n = 0 };

function addObject(o)
    objects[objects.n] = o;
    o.index = objects.n;
    objects.n = objects.n + 1;
    return o;
end

layers = { n = 0; }

function addLayer(l)
	layers[layers.n] = l;
	layers.n = layers.n + 1;
end

function layerAddImage(l,fn, x, y, w, h)
	l.n = l.n or 0;
	l[l.n] = {
		filename = fn;
		posX = x;
		posY = y;
		imgW = w;
		imgH = h;
	}
	l.n = l.n + 1;
end


function objectAddImage(obj,fn, x, y, w, h,opt)
	obj.images = obj.images or {};
	obj.images.n = obj.images.n or 0;
	opt = opt or {};
	opt.filename = fn;
	opt.posX = x;
	opt.posY = y;
	opt.imgW = w;
	opt.imgH = h;
	obj.images[obj.images.n] = opt;
	obj.images.n = 	obj.images.n + 1;
end

function objectAddBox(obj,x, y, w, h,proto)
	obj.physics = obj.physics or {};
	obj.physics.n = obj.physics.n or 0;
	local tmp = proto or {};
	obj.physics[obj.physics.n] = tmp;
	tmp.shape = "box";
	tmp.boxH = h;
	tmp.boxW = w;
	tmp.boxX = x;
	tmp.boxY = y;
	obj.physics.n = obj.physics.n + 1;	
end

function objectAddCircle(obj,r,proto)
	obj.physics = obj.physics or {};
	obj.physics.n = obj.physics.n or 0;
	tmp = proto or {};
	obj.physics[obj.physics.n] = tmp;
	tmp.shape = "circle";
	tmp.circleR = r;
	obj.physics.n = obj.physics.n + 1;	
end

Initializers = {}
function Initializers.PHLObject(obj)
	obj.images = { n = 0; };
	obj.physics = { n = 0; }
end
function Initializers.PHLCamera(obj)
	Initializers.PHLObject(obj)
end
function Initializers.PHLPlayer(obj)
	Initializers.PHLObject(obj)
	obj.physics.dynamic = true;
	objectAddCircle(obj,0.25,{restitution = 0.6;});
	obj.maxVelocityX = 3;
	objectAddImage(obj,"/ball.png",-0.25,-0.25,0.5,0.5,{tag=20; class="PHTrailImageView"});
    objectAddImage(obj,"/face.png",-0.25,-0.25,0.5,0.5);
end

function objectWithClass(class)
	obj = {};
	obj.class = class;
	if (Initializers[class]) then
		Initializers[class](obj);
	else
		Initializers.PHLObject(obj);
	end
	return obj;
end

JointInitializers = {}

function JointInitializers.PHJoint(joint)
end

function jointWithClass(class)
    joint = {};
    joint.class = class;
    if (JointInitializers[class]) then
        JointInitializers[class](joint);
    else
        JointInitializers.PHJoint(joint);
	end
	return joint
end

joints = { n = 0; };

function addJoint(o)
    joints[joints.n] = o;
    o.index = joints.n;
    joints.n = joints.n + 1;
    return o;
end

function colorWithRGBA(r,g,b,a)
    return {
        r = r or 1.0;
        g = g or 1.0;
        b = b or 1.0;
        a = a or 1.0;
    };
end

function point(x,y)
    return {
        x = x or 0;
        y = y or 0;
    };
end

function rect(x,y,w,h)
    return {
        x = x or 0;
        y = y or 0;
        width = w or 0;
        height = h or 0;
    };
end

-- log bridges

function angleToPoint(r,c,a)
	local pnt = {};
	pnt.x = c.x + r*math.cos(a);
	pnt.y = c.y + r*math.sin(a);
	return pnt;
end

function pointToAngle(r,c,p)
	local ang;
	local cos = p.x-c.x;
 	local sin = p.y-c.y;
	local d = math.sqrt(cos^2+sin^2);
	cos = cos/d;
	sin = sin/d;
	if (math.abs(sin)<0.5) then
		ang = math.asin(sin);
		if (cos<0) then
			ang = math.pi-ang;
		end
	else
		ang = math.acos(cos);
		if (sin<0) then
			ang = 2*math.pi-ang;
		end
	end
	if (ang<0) then
		ang = ang + 2*math.pi;
	end
	return ang;
end

function createBridge(bridgeRadius, p1,p2 ,numLogs, jointPos, creator)
	local q = math.sqrt((p1.x-p2.x)^2+(p1.y-p2.y)^2);
	local c1 = {};
	local c2 = {};
	local m = {};
	m.x = (p1.x+p2.x)/2;
	m.y = (p1.y+p2.y)/2;
	c1.x = m.x + math.sqrt(bridgeRadius^2-(q/2)^2)*(p1.y-p2.y)/q;
	c1.y = m.y + math.sqrt(bridgeRadius^2-(q/2)^2)*(p2.x-p1.x)/q;
	c2.x = m.x - math.sqrt(bridgeRadius^2-(q/2)^2)*(p1.y-p2.y)/q;
	c2.y = m.y - math.sqrt(bridgeRadius^2-(q/2)^2)*(p2.x-p1.x)/q;
	local c = c1;
	if (c1.y>c2.y) then
		c = c1;
	else
		c = c2;
	end
	
	local a1 = pointToAngle(bridgeRadius,c,p1);
	local a2 = pointToAngle(bridgeRadius,c,p2);
	while (a2<=a1) do
		a2=a2+math.pi*2;
	end
	
	local add = (a2-a1)/(numLogs-1);
	
	local first = nil;
	local last = nil;
	
	while (numLogs ~= 0) do
		local pnt = angleToPoint(bridgeRadius,c,a1);
		local obj = creator();
		obj.posX = pnt.x;
		obj.posY = pnt.y;
		obj.rotation = 280-(a1/math.pi*180);
		a1=a1+add;
		numLogs=numLogs-1;
		if (last) then
			local joint = jointWithClass("PHDistanceJoint");
			joint.body1 = last;
			joint.body2 = obj;
			joint.anchor1 = {x=jointPos; y=0;};
			joint.anchor2 = {x=-jointPos; y=0;};
			addJoint(joint);
		else
			first = obj;
		end
		last = obj;
	end
	return first,last;
end

