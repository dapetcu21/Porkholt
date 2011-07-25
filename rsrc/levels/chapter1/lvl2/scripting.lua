player:setUserInput(false);
player:setBraked(true);

timer = PHTimer:new(nil,1,false)
timer:setCallback(function()
player:addDialog("What is this feeling inside me?");
player:addDialog("I feel... Lighter. As if I could leave the ground behind me anytime",function ()
player:setUserInput(true);
player:setBraked(false);
sign1:display();
end);
end);
timer:schedule();

function shieldpup:dismissed()
	sign2:display();
end