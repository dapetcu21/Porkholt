function sensor:objectEntered()
	if (sensor.beenhere) then
		return;
	end
	sensor.beenhere = true;
	player:setFreezed(true);
	sign:display(function()
	player:setFreezed(false);
	end);
end