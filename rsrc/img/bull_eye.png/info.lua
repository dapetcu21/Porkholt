section("idle");
frame(0,100)
jump(idle,0);

section("charging");
frame(0,0);
fade(1,0.3);

section("relaxing");
frame(1,0);
fade(0,1);

default("idle");