
local names = {}

local _M = {}
local function set_name(name)

	local entry = names[name]
	local count = 0
	if entry == nil then
		names[name] = {name=name,count=1}
		count = 1
	else
		names[name]["count"] = names[name]["count"] + 1
		count = names[name]["count"]
	end
	
	return name .. "_" .. count 
end

_M.set_name = set_name
return _M


