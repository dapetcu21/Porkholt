timer = PHTimer:new(nil,1,false)
timer:setCallback(function()
player:addDialog("What is this feeling inside me?");
player:addDialog("I feel... Lighter. As if I could leave the ground behind me anytime",sign1.display,sign1);
end);
timer:schedule();