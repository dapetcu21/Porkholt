mayor:setDynamic(false);
player:setBraked(true);
player:setUserInput(false);

function afterDialogs()
	player:setBraked(false);
	player:setUserInput(true);
end

function  addDialogs()
	mayor:addDialog("Benno, I know that you are a good guy and I know that your actions are noble in origin...");
	mayor:addDialog("But the results are destroying this city and I, as the mayor of Beansville...");
	mayor:addDialog("...feel responsible to firstly assure the safety of the citizens");
	mayor:addDialog("That's why I'm gonna give you one last chance"); --,function() PHWorld:overlayText("Chapter I:\nORIGINS",3.0); end);
	mayor:addDialog([[As you know, Beansville's energy source is a shoug elixir of ??? origins simply called "The Goo"]])
	mayor:addDialog("Every year, the pipes that transport the Goo build up a lot of pressure")
	mayor:addDialog("Your job is simple: Release the pressure valve.");
	mayor:addDialog("AND DON'T TOUCH ANYTHING ELSE!");
	player:addDialog("Yeah! That sounds simple enough!")
	mayor:addDialog("And one last thing: You can tilt your device to move around",afterDialogs)
	mayor:setShowsQuest(true);
end

function mayor:questTapped()
	mayor:addDialog("Tilt your device to move");
end

local timer = PHTimer:new(nil,1,false);
timer:setCallback(addDialogs);
timer:schedule();