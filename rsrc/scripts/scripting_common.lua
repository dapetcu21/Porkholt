require("init_common");

objects = nil;
layers = nil;
addLayer = nil;
layerAddImage = nil;
jointWithClass = nil;
addJoint = nil;
addObject = function (o) return PHWorld:insertObject(o) end

function PHLog(fmt, ...)
	print(string.format("Porkholt: "..fmt,unpack(arg)));
end

PHWorld = {}
function PHWorld:insertObject(o) --not a good idea to batch-create objects mid-level, especially if not inserted at the end
	local ud = self._insertObj;
	if ud then
		ud = ud.ud;
	end
	return self:_insertObject(o,self._insertPos,ud);
end
function PHWorld:insertAtTheEnd()
	self._insertPos = 0;
	self._insertObj = nil;
end
function PHWorld:insertAtTheBeggining()
	self._insertPos = 1;
	self._insertObj = nil;
end
function PHWorld:insertBefore(o)
	self._insertPos = 2;
	self._insertObj = o;
end
function PHWorld:insertAfter(o)
	self._insertPos = 3;
	self._insertObj = o;
end
PHWorld:insertAtTheEnd();

PHLObject = {}
function PHLObject:new(o,ud, ...)
	o = o or {}
	o.ud = ud
	setmetatable(o,self)
	self.__index = self
	o = o:init(unpack(arg));
	return o
end
function PHLObject:init()
	return self;
end
-- function PHLObject:rotation();
-- function PHLObject:position();
-- function PHLObject:transform();
-- function PHLObject:setRotation(r);
-- function PHLObject:setPosition(p);
-- function PHLObject:setTransform(p,r);
-- function PHLObject:move(p);
-- function PHLObject:rotateAround(r,a);
-- function PHLObject:rotate(r);
-- function PHLObject:isDynamic();
-- function PHLObject:setDynamic(d);
-- function PHLObject:destroy();
-- function PHLObject:invalidateAllAnimations();
-- function PHLObject:skipAllAnimations();
-- function PHLObject:addAnimation(anim);
-- function PHLObject:applyImpulse(impulse,applicationPoint)
-- function PHLObject:velocity()
-- function PHLObject:scalarVelocity()
-- function PHLObject:setVelocity(vel) -- vector
-- function PHLObject:angularVelocity()
-- function PHLObject:setAngularVelocity(val) -- number
-- function PHLObject:applyAngularImpulse(val) -- number
-- function PHLObject:mass();
-- function PHLObject:centerOfMass(); --in object coordinates

PHObject = {};
function PHObject:new(o, ...)
	o = o or {}
	setmetatable(o,self);
	self.__index = self
	o = o:init(unpack(arg));
	return o;
end
function PHObject:init()
	return self;
end

PHTimer = PHObject:new{time = 0; willrepeat = false;}
function PHTimer:init(tm,rep,cb, ...)
	if (PHObject.init(self)) then
		self.time = tm or 0;
		self.willrepeat = rep or false;
		self.callback = cb or nil;
		self.args = arg;
		return self;
	end
	return nil;
end
function PHTimer:setCallback(cb, ...)
	self.callback = cb or nil;
	self.args = arg;
end
function PHTimer:timerFired()
	local cb = self.callback;
	local args = self.args or {};
	if (cb) then
		cb(unpack(args));
	end
end
--function PHTimer:invalidate()
--function PHTimer:schedule(timer)

PHLAnimation = PHObject:new{
	LinearFunction = 0;
	BounceFunction = 1;
	FadeInFunction = 2;
	FadeOutFunction = 3;
	FadeInOutFunction = 4;
	ConstantFunction = 5; --use this for forces and velocities
	curveFunction = LinearFunction;
};
--PHLAnimation.time --the duration of the animation
--PHLAnimation.movement
---
--PHLAnimation.rotation
--PHLAnimation.rotationCenter -- in world coordinates or object coordinates if objectCoordinates is true
---
--PHLAnimation.velocity --tries to maintain this vectorial velocity using a maximum force of correctorForce, can be applied infinitely
--PHLAnimation.correctorForce --defaults to infinity, don't leave it at infinity unless you want instant velocity correction
---
--PHLAnimation.force --can be applied infinitely
--PHLAnimation.impulse --same as force if LinearFunction, allows for better distribution control
--PHLAnimation.angularImpulse
--PHLAnimation.objectCoordinates --apply the force/impulse relative to the object coordinate system, does not affect the application point, defaults to false
--PHLAnimation.forceApplicationPoint --the force/impulse application point
---
--PHLAnimation.curveFunction --this can be one of the built-in functions defined as numbers above or an outright function
--PHLAnimation.nextAnimation --you can chain animations
--PHLAnimation.disableDynamics --disable physics for the object while animated... defaults to true
--function PHLAnimation:invalidate(); --this leaves the animation as it is and cancells it (including its callback)
--function PHLAnimation:skip(); --this applies all changes immediately, except for forces and velocities
--function PHLAnimation:invalidateChain();
--function PHLAnimation:skipChain();
---
--PHLAnimation.callbackOnInvalidate --normally, calling invalidate() on an animation cancels the callback, set this to true to override that behaviour
function PHLAnimation:setCallback(cb,...)
	self.cb = cb;
	self.args = arg;
end
function PHLAnimation:animationFinished() --don't call this manually
	if self.cb then
		self.cb(unpack(self.args or {}));
	end
end

PHLNPC = PHLObject:new();
--function PHLNPC:isFlipped();
--function PHLNPC:setFlipped(f);
--function PHLNPC:flip();
--function PHLNPC:hasTrail();
--function PHLNPC:setTrail(f);
--function PHLNPC:usesTrail();
--function PHLNPC:setUsesTrail(f);

PHLPlayer = PHLNPC:new()

PHLCamera = PHLObject:new()

PHLSensor = PHLObject:new()
function PHLSensor:objectEntered(obj) --override these. obj is either a table representing an object or a string with the object's class if the object isn't scriptable
end
function PHLSensor:objectExited(obj)
end