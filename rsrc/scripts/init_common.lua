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
	tmp = proto or {};
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
	objectAddImage(obj,"ball.png",-0.25,-0.25,0.5,0.5);
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