--
-- Primitives
--
-- local prim = <Primitive>:new(name,<opt>)
-- local mud = prim.mud
--

require "Class"

local Util = require "Util"
local Curve = require "Curve"

local Circle = {}
local Cube = {}
local Sphere = {}
local Mastaba = {}
local Quad = {}
local Spline = {
	degree=3,
	dimension=3,
	knots_count=0,
	points_count=0,
	resolution=3,
	knots={},		-- ctrl points
	points={},		-- eval points
}

local _M = _M or {} 

local db = false

-----------------------------------------------------
-- Spline 
-----------------------------------------------------

function Spline:new()

	-- Based on Spline Table (see up)
	local spline = {}
	local name = Util.set_name("spline")

	-- Set Mud proto
	setproto(spline,self,name)

	spline.knots= {}
	spline.points = {}

	return spline
end

-- Add a knot
function Spline:add(x,y,z)

	self.knots_count = self.knots_count + 1 
	self.knots[self.knots_count] = {x,y,z}
end

--- Init: Add knots to C struct 
function Spline:init()

	for k,v in pairs(self.knots) do
		self.spline:set_point(k,v[1],v[2],v[3])
	end

	self.init = true
end

-- Calculate points on curve
function Spline:make()

	self.spline = spline.new(self.degree,self.dimension,self.knots_count)

	if self.knots_count > self.degree then

		-- Init
		-- Add knots to C struct
		self:init()

		-- Calculate fractions
		local f = 1 / self.resolution
		local v1 = nil

		-- Parameter from 0 to 1
		local p = 0
		for i=1,self.resolution+1 do

			local x,y,z
			-- Eval point position
			x,y,z = self.spline:eval(p)
			-- Round value
			x = Util.round(x,4)
			y = Util.round(y,4)
			z = Util.round(z,4)
			-- Store new point
			self.points[i] = smath.new(x,y,z)
			self.points_count = self.points_count + 1
			if(db) then print("p" .. i .. "@" .. round(p,2) .. " (" .. x .. "," .. y .. "," .. z .. ")") end
			-- Calculate new parameter 
			p = p + f
		end

	else
		print("[lua] Spline: point count [" .. self.knots_count .. "] must be superior to curve degree [" .. self.degree .. "]")
	end
end

-- Build a Mud Line by edges
function Spline:build()

	local p1 = nil
	for i=1,self.resolution+1 do

		local p = self.points[i]
		local p2 = self:add_vertex(p.x,p.y,p.z)

		if p1 ~= nil then
			self:add_edge(p1,p2)
		end

		p1 = p2
	end

	self.mud:build()
end

-- Calculate Point on curve at parameter (from 0 to 1)
function Spline:eval(p)

	-- Check if inited
	if self.init then
		x,y,z = self.spline:eval(p)
		return x,y,z
	-- Else init
	else
		self:make()
		-- and come back
		self:eval()
	end
end

-- Print Spline
function Spline:print()

	print(self.name)
	print("degree:" .. self.degree)
	print("resolution:" .. self.resolution)
	print("knots:" .. self.knots_count)
	for k,v in pairs(self.knots) do
		print(k .. " (" .. v[1] .. "," .. v[2] .. "," .. v[3] .. ")")
	end
	print("points:" .. self.points_count)
	for k,v in pairs(self.points) do
		print(k .. " (" .. v[1] .. "," .. v[2] .. "," .. v[3] .. ")")
	end
end

-----------------------------------------------------
-- Circle 
-----------------------------------------------------

function Circle:new(resolution,radius)

	local _circle = {}
	local name = Util.set_name("circle")

	-- Set Mud proto
	setproto(_circle,self,name)

	pt = nil
	start = nil

	points = Curve.Circle(resolution,radius)

	for k,v in pairs(points) do

		_pt = _circle:add_vertex(v[1],v[2],v[3])

		if pt ~= nil then
			e = _circle:add_edge(_pt,pt)
		end

		pt = _pt

		if start == nil then
			start = _pt
		end

	end

	-- last edge
	
	_circle:add_edge(_pt,start)
	_circle:build()

	return _circle

end

-- Sphere

function Sphere:new(radius,rx,ry)

	local sphere = {}
	local name = Util.set_name("sphere")

	-- Set Mud proto
	setproto(sphere,self,name)

	-- Fraction of angles
	local ax = math.pi * 2 / rx
	local ay = math.pi * 2 / (ry * 2) 

	-- current altitude angle
	local phi = ay
	local rho = ax

	-- Top & Bottom vertices
	local top = sphere:add_vertex(0,0,radius)
	local down = sphere:add_vertex(0,0,-radius)

	-- array of circles
	local circles = {}

	-- from pole to equator do
	-- ry - 1
	for i=1,ry do

		-- radius 
		local r = radius * math.sin(phi)

		-- height
		local h = radius * math.cos(phi)

		-- top triangles pole
		if i == 1 then

			local points = Curve.Circle(rx,r)
			local circle = {}

			for j = 1,rx do

				-- add up point 
				local p = points[j]
				circle[j] = sphere:add_vertex(p[1],p[2],h)
			end

			-- store current circle
			circles[i] = circle

			-- set faces
			for p = 1,rx do

				local a = circle[p]
				if p == rx then
					b = circle[1]
				else
					b = circle[p+1]
				end

				-- hack
				if b and top and a then
				--sphere:add_face(b,top,a)
				end
			end

		-- bottom triangles pole
		elseif i == ry then

			local circle = circles[i-1]

			-- set faces
			for p = 1,rx do

				local a = circle[p]
				if p == rx then
					b = circle[1]
				else
					b = circle[p+1]
				end

				--sphere:add_face(a,down,b)
			end

		-- quads
		else

			local points = Curve.Circle(rx,r)
			local circle = {}

			for j = 1,rx do

				local p = points[j]

				-- add up point 
				circle[j] = sphere:add_vertex(p[1],p[2],h)
			end

			circles[i] = circle

			for p = 1,rx do

				local a,b,c,d
				local previous = circles[i-1]
				local current  = circles[i]

				a = current[p]

				if p == rx then
					b = current[1]
					c = previous[1]
				else
					b = current[p+1]
					c = previous[p+1]
				end

				d = previous[p]

				-- hack
				--sphere:add_face(a,b,c,d)

			end
		end

		phi = phi + ay
	end

	--sphere:build()

	return sphere

end

-- Quad

function Quad:new()

	local _quad = {}
	local name = Util.set_name("quad")

	-- Set Mud proto
	setproto(_quad,self,name)

	local d = 2.1
	local a = _quad:add_vertex(0+d,0,0)
	local b = _quad:add_vertex(1+d,0,0)
	local c = _quad:add_vertex(1+d,1,0)
	local d = _quad:add_vertex(0+d,1,0)

	_quad:add_face(a,b,c,d)
	_quad:build()

	return _quad
end

-- Cube

function Cube:new(x,y,z)

	local cube =  {}
	local name = Util.set_name("cube")

	-- Set Mud proto
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

function Mastaba:new( w, h, z, count)

	local mastaba = Mastaba 
	local name = Util.set_name("mastaba")

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

_M.Quad = Quad
_M.Circle = Circle
_M.Cube = Cube
_M.Mastaba = Mastaba
_M.Spline = Spline
_M.Sphere = Sphere

return _M


