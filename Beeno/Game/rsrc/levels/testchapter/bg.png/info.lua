section("red");
frame(0,0);
fade(3,1);
frame(3,function() return 1+math.random() end); --wait somewhere between 1 and 2 seconds
jump("red",0); --repeat the animation (jump to frame 0 of section red)

section("white");
frame(3,0.2);
frame(4,0.2);
frame(5,0.2);
frame(6,0.2);
jump("red",0);

default("white");