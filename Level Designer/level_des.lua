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
    obj.index = nil;
	
	des = {};
	if (obj.levelDes) then
		des.readOnly = false;
	end
	obj.levelDes = nil;
	local j = 0;
	proto = objectWithClass(obj.class)
	proto.class = nil;
	proto.posX = nil;
	proto.posY = nil;
	proto.rotation = nil;
	for i,v in pairs(obj) do
        if (type(v)=="table") then
            des[j] = { key = i;};
            des[j].value,des[j].inherited = describeTable(obj[i],proto[i]);
        else
            des[j] = { key = i; value = v; };
            if (proto[i]==v) then
                des[j].inherited = true;
            end
        end
		j = j+1;
	end
	des.n = j;
	return des;
end

function describeTable(obj,proto)
    print("describeObject",obj,proto);
    local inh = true;
    local des = {};
    local j = 0;
    proto = proto or {};
    for i,v in pairs(obj) do
	    print(i,v,type(v))
        if (type(v)=="table") then
            des[j] = { key = i;};
            des[j].value,des[j].inherited = describeTable(obj[i],proto[i]);
        else
            des[j] = { key = i; value = v; };
            if (proto[i]==v) then
                des[j].inherited = true;
            else
                inh = false;
            end;
        end
        j = j+1;
    end
    des.n = j;
    print(des,des.n);
	return des,inh;
end