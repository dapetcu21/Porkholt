require("init_common");

objects = nil;
layers = nil;
addLayer = nil;
layerAddImage = nil;
jointWithClass = nil;
addJoint = nil;
addObject = function (o) return PHWorld:insertObject(o) end

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


PHLPlayer = PHLObject:new()
function PHLPlayer:init() --constructor example
	if (PHLObject.init(self)) then
		-- extra init code here
		return self;
	end
	return nil;
end

PHLCamera = PHLObject:new()