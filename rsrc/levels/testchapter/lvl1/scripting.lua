print(wrecker.ud);

function callback1(x)
	print("timer 1 fired");
end

function callback2()
	print("timer 2 fired");
	timer1:invalidate();
end

timer1 = PHTimer:new(nil,1,true);
timer1:setCallback(callback1);
timer1:schedule();

timer2 = PHTimer:new(nil,10.5,false);
timer2:setCallback(callback2);
timer2:schedule();