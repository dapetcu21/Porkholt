player:setFreezed(true);

timer = PHTimer:new(nil,1,false)
timer:setCallback(function()
player:addDialog("...jump",function ()
sign1:display(function()
player:setFreezed(false);
end);
end);
end);
timer:schedule();

function shieldpup:dismissed()
	sign2:display();
end

function sensordash:objectEntered()
	if (sensordash.beentheredonethat) then
		return;
	end
	sensordash.beentheredonethat = true;
	player:setFreezed(true);
	sign3:display(function()
	player:setFreezed(false);
	end);
end

function hppotion:dismissed()
	hpsign:display();
end