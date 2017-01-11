require "Class"
local Curve = require "Curve"
local Circle = {}
local Cube = {}
local Mastaba = {}
local _M = _M or {} 

-- Circle

function Circle:new(name,resolution,radius)

	local _circle = {}

	-- Set Stone proto
	setproto(_circle,self,name)

	pt = nil
	start = nil

	points = Curve.circle(resolution,radius)

	for k,v in pairs(points) do
		print(v.x)
		print(v.y)

		_pt = _circle:add_vertex(v.x,v.y,0)

		if pt ~= nil then
			e = _circle:add_edge(_pt,pt)
		end

		pt = _pt

		if start == nil then
			print("add")
			start = _pt
		end

	end

	-- last edge
	
	_circle:add_edge(_pt,start)
	_circle:build_object()

	return _circle

end

-- Cube

function Cube:new(name,x,y,z)

	local cube =  {}

	-- Set Stone proto
	setproto(cube,self,name)

	cube.x = x
	cube.y = y 
	cube.z = z

	a = cube:add_vertex(0,0,0)
	b = cube:add_vertex(x,0,0)
	c = cube:add_vertex(x,y,0)
	d = cube:add_vertex(0,y,0)
	f = cube:add_face(a,b,c,d)
	cube:extrude_face(f,0,0,z)

	return cube
end

function Cube:print()
	print(self)
	print("Cube (" .. self.x .. " " .. self.y .. " " .. self.z .. ")")
	self.__proto__:print()
end


-- Mastaba

function Mastaba:build_layer(va,vb,vc,vd)

	-- var
	offset = .8
	
	v0 = va:copy()
	v0:sub(vb)
	height = v0:length() * self.z

	-- center by diagonal ac
	vcenter = vc:copy()
	vcenter:sub(va)
	vcenter:mul(.5)

	center = va:copy()
	center:add(vcenter)

	-- semi-vectors
	v1 = va:copy()
	v1:sub(center)

	v2 = vb:copy()
	v2:sub(center)

	v3 = v1:copy()
	v3:mul(-1)

	v4 = v2:copy()
	v4:mul(-1)

	v1:mul(offset)
	v2:mul(offset)
	v3:mul(offset)
	v4:mul(offset)

	-- z
	vz = smath.new(0,0,height)

	-- a,b,c,d
	a = self:add_vertex(va.x,va.y,va.z)
	b = self:add_vertex(vb.x,vb.y,vb.z)
	c = self:add_vertex(vc.x,vc.y,vc.z)
	d = self:add_vertex(vd.x,vd.y,vd.z)

	-- e,f,g,h
	ve = va:copy()
	vf = vb:copy()
	vg = vc:copy()
	vh = vd:copy()

	ve:add(vz)
	vf:add(vz)
	vg:add(vz)
	vh:add(vz)

	e = self:add_vertex(ve.x,ve.y,ve.z)
	f = self:add_vertex(vf.x,vf.y,vf.z)
	g = self:add_vertex(vg.x,vg.y,vg.z)
	h = self:add_vertex(vh.x,vh.y,vh.z)

	-- i, j, k, l
	vi = center:copy()
	vj = center:copy()
	vk = center:copy()
	vl = center:copy()

	vi:add(vz)
	vj:add(vz)
	vk:add(vz)
	vl:add(vz)

	vi:add(v1)
	vj:add(v2)
	vk:add(v3)
	vl:add(v4)

	i = self:add_vertex(vi.x,vi.y,vi.z)
	j = self:add_vertex(vj.x,vj.y,vj.z)
	k = self:add_vertex(vk.x,vk.y,vk.z)
	l = self:add_vertex(vl.x,vl.y,vl.z)

	-- faces
	self:add_face(a,b,f,e)
	self:add_face(b,c,g,f)
	self:add_face(c,d,h,g)
	self:add_face(d,a,e,h)

	self:add_face(e,f,j,i)
	self:add_face(f,g,k,j)
	self:add_face(g,h,l,k)
	self:add_face(h,e,i,l)

	-- return new base
	return vi,vj,vk,vl

end

function Mastaba:build_layers(va,vb,vc,vd)

	-- build first layer
	a,b,c,d = self:build_layer(va,vb,vc,vd)

	for i=1,self.count-1 do
		a,b,c,d = self:build_layer(a,b,c,d)
	end

end

function Mastaba:new( name, w, h, z, count)

	local mastaba = Mastaba 

	setproto(mastaba,self,name)

	mastaba.w = w
	mastaba.h = h
	mastaba.count = count
	mastaba.z = z

	-- initial points
	va = smath.new(0,0,0)
	vb = smath.new(w,0,0)
	vc = smath.new(w,h,0)
	vd = smath.new(0,h,0)

	self:build_layers(va,vb,vc,vd)

	return mastaba
end

_M.Circle = Circle
_M.Cube = Cube
_M.Mastaba = Mastaba

return _M


