--local stone = require "stone"
--local math = require "stdmath"
--local math = stdmath
local s = stone.new("s")
local wrapper = {}
local Cube = {}
local Mastaba = {}
local _M = _M or {} 

-- Wrapper

function wrapper:__index(key)

	-- Get self
	_self = rawget(self,"__self__")
	local f = rawget(_self,key)

	-- Object's self functions
	if f then
		return function (me,...)
			return f(me,...)
		end
	else
		-- Stone's functions

		local proto = rawget(self,"__proto__")
		local field = proto and proto[key]
		if type(field) ~= "function" then
			return field
		else
			return function(obj,...)
				if obj == self then
					return field(proto,...)
				else
					return field(obj,...)
				end
			end
		end
	end
	
end

function setproto(o,self,name)
	setmetatable(o,wrapper)
	o.__proto__ = stone.new(name)
	o.__self__ = self
end

-- Cube

function Cube:hello()
	print("hello")
end

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

function get_points(vec)
	return vec:get(VEC_X), vec:get(VEC_Y), vec:get(VEC_Z)
end

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
	vz = math.new(0,0,height)

	-- a,b,c,d
	a = self:add_vertex(get_points(va))
	b = self:add_vertex(get_points(vb))
	c = self:add_vertex(get_points(vc))
	d = self:add_vertex(get_points(vd))

	-- e,f,g,h
	ve = va:copy()
	vf = vb:copy()
	vg = vc:copy()
	vh = vd:copy()

	ve:add(vz)
	vf:add(vz)
	vg:add(vz)
	vh:add(vz)

	e = self:add_vertex(get_points(ve))
	f = self:add_vertex(get_points(vf))
	g = self:add_vertex(get_points(vg))
	h = self:add_vertex(get_points(vh))

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

	i = self:add_vertex(get_points(vi))
	j = self:add_vertex(get_points(vj))
	k = self:add_vertex(get_points(vk))
	l = self:add_vertex(get_points(vl))

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
	va = math.new(0,0,0)
	vb = math.new(w,0,0)
	vc = math.new(w,h,0)
	vd = math.new(0,h,0)

	self:build_layers(va,vb,vc,vd)

	return mastaba
end


_M.Cube = Cube
_M.Mastaba = Mastaba

return _M


