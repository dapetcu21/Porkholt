mayor:setDynamic(false);
player:setBraked(true);
player:setUserInput(false);
player.initialPosition = player:position();
mayor.initialPosition = mayor:position();
camera.initialPosition = camera:position();


function sign:questTapped()
	sign:setDialog("Tilt your device\nto move around");
end

function afterDialogs()
	player:setBraked(false);
	player:setUserInput(true);
	sign:setDialog("Tilt your device\nto move around",sign.setShowsQuest,sign,true)
end

function  addDialogs()
	mayor:addDialog("Benno, I know that you are a good guy and I know that your actions are noble in origin,");
	mayor:addDialog("but the results are destroying this city and I, as the mayor of Beansville,");
	mayor:addDialog("feel responsible to firstly assure the safety of the citizens.");
	mayor:addDialog("That's why I'm gonna give you one last chance"); --,function() PHWorld:overlayText("Chapter I:\nORIGINS",3.0); end);
	mayor:addDialog([[As you know, Beansville's energy source is a strong elixir of unknown origins simply called "The Goo"]])
	mayor:addDialog("Every year, the pipes that transport the Goo build up a lot of pressure")
	mayor:addDialog("Your job is simple: Release the pressure valve.");
	mayor:addDialog("AND DON'T TOUCH ANYTHING ELSE!");
	player:addDialog("Yeah! That sounds simple enough!",afterDialogs);
end

function faded1()
	player:setVelocity(vector(0,0));
	player:setAngularVelocity(0);
	player:setPosition(marker1:position());
	camera:setPosition(marker3:position());
	camera:setFollowsPlayer(false);
	PHWorld:dismissFading();
end

function endalldialogs()
	player:setBraked(false);
	player:setUserInput(true);
end

function faded3ended()
	mayor:addDialog("That's it! You've outdone yourself!");
	mayor:addDialog("You'll leave this city right now and never return again!");
	player:addDialog("Wait! Wait!\nI'm sorry!\nBut I can...");
	mayor:addDialog("GET OUT!",endalldialogs);
end

function _faded3()
	player:setVelocity(vector(0,0));
	player:setAngularVelocity(0);
	player:setPosition(player.initialPosition);
	mayor:setPosition(mayor.initialPosition);
	camera:setPosition(camera.initialPosition);
	camera:setFollowsPlayer(true);
	player:setFlipped(true);
	mayor:setFlipped(false);
	sign:destroy();
	PHWorld:dismissFading(faded3ended);
	
	local obj = objectWithClass("PHLLevelEnd");
	obj.pos = sensor1:position();
	objectAddBox(obj,-0.25,-0.25,0.5,0.5);
	PHWorld:insertObject(obj);
	sensor1:destroy();
end

function faded3()
	PHWorld:fadeToColor(colorWithRGBA(0,0,0,1),_faded3);
end

function walked()
	mayor:setDynamic(false);
	player:setFlipped(false);
	mayor:addDialog("Beeno, what have you done!");
	mayor:addDialog("You're red and the whole town is out of power!");
	player:addDialog("Yeah.. Well... ummm...");
	player:addDialog("I kinda screwed up and it all blew up",faded3);
end

function faded2()
	player:viewWithTag(20):setImage("ball");
	player:setVelocity(vector(0,0));
	player:setAngularVelocity(0);
	player:setFlipped(false);
	PHWorld:dismissFading();
	mayor:setPosition(marker2:position());
	piperoomdoor:destroy();
	mayor:setDynamic(true);
	mayor:walkTo(marker1:position(),2,walked);
end

function sensor2:objectEntered()
	player:setBraked(true);
	player:setUserInput(false);
	PHWorld:fadeToColor(colorWithRGBA(0.9,0.1,0.1,1),faded2);
end

function sensor1:objectEntered()
	PHWorld:fadeToColor(colorWithRGBA(0,0,0,1),faded1);
end

local timer = PHTimer:new(nil,1,false);
timer:setCallback(addDialogs);
timer:schedule();