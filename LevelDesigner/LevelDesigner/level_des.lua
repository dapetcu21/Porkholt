require("init_common");

function describeObject(obj)
	obj.class = obj.class or "PHLObject";
	obj.pos = obj.pos or {};
    obj.pos.x = obj.pos.x or 0;
    obj.pos.y = obj.pos.y or 0;
	obj.rotation = obj.rotation or 0;
    local oldIndex = obj.index;
    local oldRealClass = obj.realClass;
    obj.index = nil;
    obj.realClass = nil;
	
    local des = { readOnly = true };
	if (obj.levelDes) then
		des.readOnly = false;
	end
	obj.levelDes = nil;
	beziers = { n=0 };
	des.rootProperty = { key = "__root__"; value = describeTable(obj); }
	des.bezierPaths = beziers;
	beziers = nil;
    obj.index = oldIndex;
    obj.realClass = oldRealClass;
	return des
end

function describeTable(obj)
    local des = {};
    local j = 0;
    local n = obj.n;
    if (type(obj.points)=="table" and type(obj.curves)=="table") then
        local i = beziers.n;
        beziers[i] = obj;
        beziers.n = i+1;
        return i
    end
    if (n) then
        des.array = true;
        des.n = n;
        for j=0,n-1 do
            v = obj[j];
            if (type(v)=="table") then
                des[j] = { key = j;};
                des[j].value = describeTable(obj[j]);
            else
                des[j] = { key = j; value = v; };
            end
        end
    else
        for i,v in pairs(obj) do
			if ( not (type(i)=="string" and string.sub(i,1,1)=="_") ) then
            	if (type(v)=="table") then
	                des[j] = { key = i;};
	                des[j].value = describeTable(obj[i]);
	            else
	                des[j] = { key = i; value = v; };
	            end
	            j = j+1;
			end
        end
        des.n = j;
    end
	return des;
end