objects = { n = 0 };

function addObject(o)
    objects[objects.n] = o;
    objects.n = objects.n + 1;
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
	print("hi",fn," ",x," ",y," ",w," ",h);
	l.n = l.n + 1;
end


function objectAddImage(obj,fn, x, y, w, h)
	obj.images = obj.images or {};
	obj.images.n = obj.images.n or 0;
	obj.images[obj.images.n] = {
		filename = fn;
		posX = x;
		posY = y;
		imgW = w;
		imgH = h;
	}
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
	obj.physics.dynamic = 1;
	objectAddCircle(obj,0.25,{restitution = 0.5;});
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