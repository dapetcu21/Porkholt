local time = 0.5;
local frames = 10;

section("poof");
for i=0,frames-1 do
  frame(i,time/frames);
end

default("poof");
