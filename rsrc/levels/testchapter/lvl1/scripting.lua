function callback()
	PHLog("timer fired");
	local boxW = ((792-29-12)/792)*2;
	local posBox = (29/792-0.5)*2;
	local obj = objectWithClass("PHLObject");
	obj.pos = player:position();
	obj.pos.y = obj.pos.y + 2;
	objectAddImage(obj,"box.png",-0.25,-0.25,0.5,0.5);
	objectAddBox(obj,-0.25,-0.25,0.5,0.5);
	obj.levelDes = true;
	obj.physics.dynamic = true;
	box = PHWorld:insertObject(obj);
	
	local timer = PHTimer:new(nil,1,true);
	timer:setCallback(callback2);
	timer:schedule();
end

function callback2()
	PHLog("timer 2 fired");
	local p = player:position();
	p.y = p.y + 2;
	box:setPosition(p);
	box:setVelocity(vector(0,0));
	box:setAngularVelocity(0);
	box:setRotation(0);
end

function callback3()
	PHLog("timer 3 fired");
	local anim = PHLAnimation:new();
	anim.rotation = 181;
	anim.rotationCenter = point(17,3.5);
	anim.time = 3;
	anim.curveFunction = function (x) return math.sin(math.pi*(x-0.5))/2 + 0.5 end --same thing as anim.curveFunction = PHLAnimation.FadeInOutFunction, only more fancy
	wrecker:addAnimation(anim);
end

function callbackulupeste(s)
	PHLog("animation callback fired: %s",s);
end

local timer = PHTimer:new(nil,1,false);
timer:setCallback(callback);
timer:schedule();

local timer = PHTimer:new(nil,10,false);
timer:setCallback(callback3);
timer:schedule();


local anim = PHLAnimation:new();
anim.force = vector(1,0);
anim.curveFunction = PHLAnimation.FadeInOutFunction;
anim.disableDynamics = false;
anim.time = 3;
anim:setCallback(callbackulupeste,"meow");
local anim2 = PHLAnimation:new();
anim2.movement = point(0,3);
anim2.rotation = 360;
anim2.time = 0.5;
anim.nextAnimation = anim2;
player:addAnimation(anim);

function sensor:objectEntered(obj)
	print("entered: ",obj);
end

function sensor:objectExited(obj)
	print("exited: ",obj);
end