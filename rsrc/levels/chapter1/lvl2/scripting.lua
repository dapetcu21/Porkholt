player:setUserInput(false);
player:setBraked(true);

timer = PHTimer:new(nil,1,false)
timer:setCallback(function()
player:addDialog("...jump",function ()
sign1:display(function()
player:setUserInput(true);
player:setBraked(false);
PHLog("hello");
end);
end);
end);
timer:schedule();

function shieldpup:dismissed()
	sign2:display();
end