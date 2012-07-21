section("idle");
frame(0,function() return 3+math.random()*3 end);
frame(1,0.1);
frame(2,0.2);
frame(1,0.1);
jump("idle",0);

section("moving");
frame(0,0);

default("idle");
