require("init_common");

function tableSubsetOfTable( tbl2, tbl1 )
    for k,v in tbl2 do
        if (tbl1[k] ~= v) then
            if ((type(tbl1[k])~="table") or (type(v)~="table")) then
                return false
            end
            if (not Loc_CoveringContents( tbl1[k], v )) then
                return false
            end
        end
    end    
    return true
end

function describeObject(obj)
	obj.class = obj.class or "PHObject";
	obj.posX = obj.posX or 0;
	obj.posY = obj.posY or 0;
	obj.rotation = obj.rotation or 0;
	
	des = {};
	if (obj.levelDes) then
		des.readOnly = false;
	end
	obj.levelDes = nil;
	j = 0;
	proto = objectWithClass(obj.class)
	proto.class = nil;
	proto.posX = nil;
	proto.posY = nil;
	proto.rotation = nil;
	for i,v in pairs(obj) do
		des[j] = { key = i; value = v; };
		if (proto[i]==v) then
			des[j].inherited = true;
		end
		j = j+1;
	end
	des.n = j;
	return des;
end