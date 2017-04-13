

-- Point count must be strictly superior to curve degree
--
-- ts_internal_bspline_new
-- if (deg >= n_ctrlp)

local P = require "Primitive"

local function s1()
	local s = P.Spline:new()
	s.degree = 3
	s.resolution = 10
	s:add(-1.75,-1,0)
	s:add(-1.5,-0.5,0)
	s:add(-1.5,0,0)
	s:add(-1.25,0.5,0)
	s:add(-0.75,0.75,0)
	s:add(0,0.5,0)
	s:add(0.5,0,0)
	--s:print()
	s:make()
	return s
end

local function s2()
	local s = P.Spline:new()
	s.degree = 3
	s.resolution = 10
	s:add(-10.75,-1,0)
	s:add(-1.5,-0.5,0)
	s:add(-1.5,0,0)
	s:add(-1.25,10.5,0)
	--s:print()
	s:make()
	return s
end

local s = s2()
local pierre = s.stone
print("Vertex count:" .. pierre.vertex_count)
for i,v in pairs(pierre.vertices) do
	print(i,v)
end

