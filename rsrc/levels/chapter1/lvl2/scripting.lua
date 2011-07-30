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
	sign3.displayed = true;
	end);
end

function sensordash:objectExited()
	if (sensordash.washeredonethat or sign3.displayed) then
		return;
	end
	sensordash.washeredonethat = true;
	player:setBraked(false);
	player:walkTo(sensordash:position(),2,function()
	if (not (sign3.displayed)) then
		player:setBraked(true);
	end
	end);
end

function hppotion:dismissed()
	hpsign:display();
end