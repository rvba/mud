

-- Point count must be strictly superior to curve degree
--
-- ts_internal_bspline_new
-- if (deg >= n_ctrlp)

require "Class"

local P = require "Primitive"
local F = require "Frame"
local U = require "Util"

local _M = _M or {} 

local edges = {}

local function build_spline()

	local s = P.Spline:new()

	s.degree = 3
	s.resolution = 10
	s:add(0,0,0)
	s:add(3,0,3)
	s:add(6,0,0)
	s:add(9,0,3)
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
		edges[i] = edge
		face = stone:extrude_edge(edge,0,3,0)
	end

	stone:build()
end

local function extrude_faces(stone)

	for i,face in pairs(stone.faces) do
		stone:extrude_face(face,0,0,-.2)
	end

	stone:build()
end

local function add_frames()

	local frames = {}
	for i,edge in pairs(edges) do

		local a = edge.a
		local b = edge.b

		ax = a.co[1]
		ay = a.co[2]
		az = a.co[3]
		bx = b.co[1]
		by = b.co[2]
		bz = b.co[3]

		local vup = smath.new(0,0,5)
		local va = smath.new(ax,ay,az)
		local vb = smath.new(bx,by,bz)
		local vc = va:copy()
		local vd = vb:copy()

		vc:add(vup)
		vd:add(vup)

		vb:sub(va)
		vc:sub(va)
		vd:sub(va)

		local frame = F.frame:new(va,vb,vc,vd)
		frames[i] = frame

		frame:construct()
	end

	return frames
end

local function build()
	local spline = build_spline()
	extrude_edges(spline.stone)
	extrude_faces(spline.stone)
	local frames = add_frames()
	return frames
end

_M.build = build

return _M




