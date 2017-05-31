-- Frame
--

require "Class"
local U = require "Util"

local _M = _M or {} 

local Frame = {
	a = 1,
	depth = .2,
	thick = .1,
}

-- b----c
-- |	|		
-- o----a	
--
-- Frame
function Frame:add_frame()

	local vo = self.vo

	-- Duplicate Points
	local va = self.va:copy()
	local vb = self.vb:copy()
	local vc = self.vc:copy()

	-- Vectorize them
	va:sub(vo)
	vb:sub(vo)
	vc:sub(vo)

	-- Duplicate Origin
	local vo = vo:copy()

	-- Build Perpendicular Cross Vector from Va and Vb
	local vcross = smath.cross(vb,va)
	vcross:norm()
	-- Give depth to it
	vcross:mul(self.depth)

	-------------------------- Outer Points 

	local vao = self.va:copy() 
	local vbo = self.vb:copy() 
	local vco = self.vc:copy() 

	-- Keep this points
	local vv2 = vco:copy()
	local vv3 = vbo:copy()

	-- Make thickness vectors
	-- FIXME wrong names
	-- FIXME vto -> vob
	local vto = vo:copy()
	vto:sub(vbo)
	vto:norm()
	vto:mul(self.thick)

	local vta = va:copy()
	vta:norm()
	vta:mul(self.thick)

	-- vbc
	local vtb = vbo:copy()
	vtb:sub(vco)
	vtb:norm()
	vtb:mul(self.thick)

	-- vca
	local vtc = vco:copy()
	vtc:sub(vao)
	vtc:norm()
	vtc:mul(self.thick)


	-------------------------- Inner Points 
	
	local v4 = vo:copy()
	v4:add(vta)
	v4:sub(vto)

	local v5 = vao:copy()
	v5:sub(vta)
	v5:add(vtc)

	local v6 = vco:copy()
	v6:sub(vtc)
	v6:add(vtb)

	local v7 = vbo:copy()
	v7:sub(vtb)
	v7:add(vto)

	-- front outer points
	local p0 = self:add_vertex(vo.x,vo.y,vo.z)
	local p1 = self:add_vertex(vao.x,vao.y,vao.z)
	local p2 = self:add_vertex(vco.x,vco.y,vco.z)
	local p3 = self:add_vertex(vbo.x,vbo.y,vbo.z)

	local pp0 = p0
	local pp1 = p1
	local pp2 = p2
	local pp3 = p3

	--front inner points
	local p4 = self:add_vertex(v4.x,v4.y,v4.z)
	local p5 = self:add_vertex(v5.x,v5.y,v5.z)
	local p6 = self:add_vertex(v6.x,v6.y,v6.z)
	local p7 = self:add_vertex(v7.x,v7.y,v7.z)

	local pp4 = p4
	local pp5 = p5
	local pp6 = p6
	local pp7 = p7

	-- front faces
	self:add_face(p0,p4,p5,p1)
	self:add_face(p1,p5,p6,p2)
	self:add_face(p2,p6,p7,p3)
	self:add_face(p3,p7,p4,p0)


	-- rear vectors
	
	vo:add(vcross)
	vao:add(vcross)
	vco:add(vcross)
	vbo:add(vcross)

	v4:add(vcross)
	v5:add(vcross)
	v6:add(vcross)
	v7:add(vcross)

	p0 = self:add_vertex(vo.x,vo.y,vo.z)
	p1 = self:add_vertex(vao.x,vao.y,vao.z)
	p2 = self:add_vertex(vco.x,vco.y,vco.z)
	p3 = self:add_vertex(vbo.x,vbo.y,vbo.z)

	p4 = self:add_vertex(v4.x,v4.y,v4.z)
	p5 = self:add_vertex(v5.x,v5.y,v5.z)
	p6 = self:add_vertex(v6.x,v6.y,v6.z)
	p7 = self:add_vertex(v7.x,v7.y,v7.z)

	self:add_face(p0,p4,p5,p1)
	self:add_face(p1,p5,p6,p2)
	self:add_face(p2,p6,p7,p3)
	self:add_face(p3,p7,p4,p0)

	-- inner sides
	--
	self:add_face(p4,p5,pp5,pp4)
	self:add_face(p5,p6,pp6,pp5)
	self:add_face(p6,p7,pp7,pp6)
	self:add_face(p7,p4,pp4,pp7)

	-- outer sides
	
	self:add_face(p0,p1,pp1,pp0)
	self:add_face(p0,p3,pp3,pp0)
	self:add_face(p3,p2,pp2,pp3)
	self:add_face(p1,p2,pp2,pp1)

end

function Frame:new(o,a,b,c) 

	local _frame = {}

	-- Set Stone proto
	setproto(_frame,self,U.set_name("frame"))

	_frame.vo = o
	_frame.va = a
	_frame.vb = b
	_frame.vc =c

	return _frame
end

function Frame:construct()
	self:add_frame()
end

_M.frame = Frame
return _M

