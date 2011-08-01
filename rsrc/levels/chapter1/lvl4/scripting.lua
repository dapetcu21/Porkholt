master:setDynamic(false);
player:setFreezed(true);

local timer = PHTimer:new(nil,1,false);
timer:setCallback(function()
	master:addDialog("Welcome to my temple, my child.\nHow can I help you?");
	player:addDialog("Well... I got kicked out of my town...",function() 
	PHWorld:curtainText("...and I told him everything that has happened to me.",function()
	master:addDialog("If you need a place to call home, this temple is forever open");
	master:addDialog("And there's one more thing:");
	master:addDialog("That Goo gave you abilities far more powerful than just jumping");
	master:addDialog("But, we'll have more time to talk about that tomorrow.");
	master:addDialog("You must be exausted. Come, get some sleep",function()
	PHWorld:win();
	end);
	end);
	end);
end);
timer:schedule();