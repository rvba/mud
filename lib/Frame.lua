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

	local depth = self.depth
	local thick = self.thick

	-- Duplicate Origin
	local vo = vo:copy()

	-- Build Perpendicular Cross Vector from Va and Vb
	local vcross = smath.cross(vb,va)
	vcross:norm()
	-- Give depth to it
	vcross:mul(depth)

	-------------------------- Outer Points 

	local v1 = self.va:copy() -- a
	local v2 = self.vc:copy() -- c
	local v3 = self.vb:copy() -- b

	-- frame
	local frame = self

	-- Keep this points
	local vv2 = v2:copy()
	local vv3 = v3:copy()

	-- Make thickness vectors
	local vt1 = va:copy()
	vt1:norm()
	vt1:mul(thick)

	local vt2 = v2:copy()
	vt2:sub(v1)
	vt2:norm()
	vt2:mul(thick)

	local vt3 = v3:copy()
	vt3:sub(v2)
	vt3:norm()
	vt3:mul(thick)

	local vt4 = vo:copy()
	vt4:sub(v3)
	vt4:norm()
	vt4:mul(thick)

	-------------------------- Inner Points 
	
	local v4 = vo:copy()
	v4:add(vt1)
	v4:sub(vt4)

	local v5 = v1:copy()
	v5:sub(vt1)
	v5:add(vt2)

	local v6 = v2:copy()
	v6:sub(vt2)
	v6:add(vt3)

	local v7 = v3:copy()
	v7:sub(vt3)
	v7:add(vt4)

	-- front outer points
	local p0 = frame:add_vertex(vo.x,vo.y,vo.z)
	local p1 = frame:add_vertex(v1.x,v1.y,v1.z)
	local p2 = frame:add_vertex(v2.x,v2.y,v2.z)
	local p3 = frame:add_vertex(v3.x,v3.y,v3.z)

	local pp0 = p0
	local pp1 = p1
	local pp2 = p2
	local pp3 = p3

	--front inner points
	local p4 = frame:add_vertex(v4.x,v4.y,v4.z)
	local p5 = frame:add_vertex(v5.x,v5.y,v5.z)
	local p6 = frame:add_vertex(v6.x,v6.y,v6.z)
	local p7 = frame:add_vertex(v7.x,v7.y,v7.z)

	local pp4 = p4
	local pp5 = p5
	local pp6 = p6
	local pp7 = p7

	-- front faces
	frame:add_face(p0,p4,p5,p1)
	frame:add_face(p1,p5,p6,p2)
	frame:add_face(p2,p6,p7,p3)
	frame:add_face(p3,p7,p4,p0)


	-- rear vectors
	
	vo:add(vcross)
	v1:add(vcross)
	v2:add(vcross)
	v3:add(vcross)

	v4:add(vcross)
	v5:add(vcross)
	v6:add(vcross)
	v7:add(vcross)

	p0 = frame:add_vertex(vo.x,vo.y,vo.z)
	p1 = frame:add_vertex(v1.x,v1.y,v1.z)
	p2 = frame:add_vertex(v2.x,v2.y,v2.z)
	p3 = frame:add_vertex(v3.x,v3.y,v3.z)

	p4 = frame:add_vertex(v4.x,v4.y,v4.z)
	p5 = frame:add_vertex(v5.x,v5.y,v5.z)
	p6 = frame:add_vertex(v6.x,v6.y,v6.z)
	p7 = frame:add_vertex(v7.x,v7.y,v7.z)

	frame:add_face(p0,p4,p5,p1)
	frame:add_face(p1,p5,p6,p2)
	frame:add_face(p2,p6,p7,p3)
	frame:add_face(p3,p7,p4,p0)

	-- inner sides
	--
	frame:add_face(p4,p5,pp5,pp4)
	frame:add_face(p5,p6,pp6,pp5)
	frame:add_face(p6,p7,pp7,pp6)
	frame:add_face(p7,p4,pp4,pp7)

	-- outer sides
	
	frame:add_face(p0,p1,pp1,pp0)
	frame:add_face(p0,p3,pp3,pp0)
	frame:add_face(p3,p2,pp2,pp3)
	frame:add_face(p1,p2,pp2,pp1)

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

