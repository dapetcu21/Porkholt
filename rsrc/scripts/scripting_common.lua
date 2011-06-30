PHWorld = {}

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
--function PHWorld:scheduleTimer(timer)


PHLPlayer = PHLObject:new()
function PHLPlayer:init() --constructor example
	if (PHLObject.init(self)) then
		-- extra init code here
		return self;
	end
	return nil;
end

PHLCamera = PHLObject:new()