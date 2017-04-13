

-- Point count must be strictly superior to curve degree
--
-- ts_internal_bspline_new
-- if (deg >= n_ctrlp)

local P = require "Primitive"

local function build_spline()

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

local function extrude_vertices(stone)

	for i,v in pairs(stone.vertices) do
		local co = v.co
		print(i,co[1],co[2],co[3])
		edge = stone:extrude_vertex(v,0,0,3)
	end

	stone:build()
end

local function extrude_edges(stone)

	for i,edge in pairs(stone.edges) do
		face = stone:extrude_edge(edge,0,0,3)
	end

	stone:build()
end

local spline = build_spline()
extrude_edges(spline.stone)



