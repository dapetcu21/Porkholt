function section(name)
	if (not nametable) then
		nametable = {}
	end
	if ((not anim) or (not(anim.n))) then
		anim = { n=0; };
	end
	pntr = { n=0; nm = name; };
	anim[anim.n] = pntr;
	anim.n = anim.n + 1;
	nametable[name] = pntr;
end

function frame(frameno,duration)
	if (not pntr) then
		section("default",false);
	end
	local frame = { fno=frameno; d=duration; };
	pntr[pntr.n] = frame;
	pntr.n = (pntr.n or 0) + 1;
end

function fade(frameno,duration)
	if (not pntr) then
		section("default",false);
	end
	local frame = { fno=frameno; d=duration; fd = true};
	pntr[pntr.n] = frame;
	pntr.n = (pntr.n or 0) + 1;
end

function jump(section,frameno)
	frame(frameno,section);
end

function default(name)
	def = nametable[name];
end