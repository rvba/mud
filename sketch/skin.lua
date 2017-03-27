--require "math"
local S = stone
local obj = S.new("skin")

local function add_edge(o,a,b)
	local pa = o:add_vertex(a[1],a[2],a[3])
	local pb = o:add_vertex(b[1],b[2],b[3])
	local edge = o:add_edge(pa,pb)
	return pb
end

for i=1,100,.1 do
	d = .1
	a = {math.cos(i), math.sin(i), i*.1}
	b = {math.cos(i+d), math.sin(i+d), i*.1}
	point = add_edge(obj,a,b)
end

obj:add_modifier_skin()
obj:build()
