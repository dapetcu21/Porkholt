PHLevelController = {}

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

PHLPlayer = PHLObject:new()
function PHLPlayer:init()
	if (PHLObject.init(self)) then
		-- extra init code here
		return self;
	end
	return nil;
end

PHLCamera = PHLObject:new()