function callback()
	print("timer fired");
	local boxW = ((792-29-12)/792)*2;
	local posBox = (29/792-0.5)*2;
	local obj = objectWithClass("PHLPlatform");
	obj.pos = point(1.5,2.75);
	objectAddBox(obj,posBox,-0.15,boxW,0.3)
	objectAddImage(obj,"platform.png",-1,-0.15,2,0.3);
	obj = PHWorld:insertObject(obj);
end

function callback2()
	print("timer2 fired");
	wrecker:destroy();
end

timer = PHTimer:new(nil,3,false);
timer:setCallback(callback);
timer:schedule();
timer = nil;

timer = PHTimer:new(nil,10,false);
timer:setCallback(callback2);
timer:schedule();
timer = nil;