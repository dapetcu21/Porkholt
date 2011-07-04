function callback()
	print("timer fired");
	local boxW = ((792-29-12)/792)*2;
	local posBox = (29/792-0.5)*2;
	local obj = objectWithClass("PHLObject");
	obj.pos = point(1.5,2.75);
	objectAddImage(obj,"box.png",-0.25,-0.25,0.5,0.5);
	objectAddBox(obj,-0.25,-0.25,0.5,0.5);
	obj.levelDes = true;
	obj.physics.dynamic = true;
	box = PHWorld:insertObject(obj);
end

function callback2()
	print("timer 2 fired");
	box:move(point(0,10));
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
