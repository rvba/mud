local _M = _M or {} 

function circle(resolution,radius)

	angle = math.pi * 2 / resolution
	a = 0
	pt = nil
	start = nil
	circle = {}

	for i = 1,resolution
	do
		local p = {}
		x = math.cos(a) * radius
		y = math.sin(a) * radius
		p.x = x
		p.y = y

		table.insert(circle,p)

		pt = _pt
		a = a + angle

	end
	return circle
end

_M.circle = circle

return _M


--[[
function circle(resolution,radius)

	circle = stone.new("circle")

	angle = math.pi * 2 / resolution
	a = 0
	pt = nil
	start = nil

	for i = 1,resolution
	do
		x = math.cos(a) * radius
		y = math.sin(a) * radius
		z = 0

		_pt = circle:add_vertex(x,y,z)

		if pt ~= nil then
			e = circle:add_edge(_pt,pt)
		end

		pt = _pt
		a = a + angle

		if start == nil then
			print("add")
			start = _pt
		end
	end

	-- last edge
	circle:add_edge(_pt,start)

	circle:build_object()
	return circle
end


function v(i,x,y,z)
	j = math.cos( time() * (i+1) *.1) * .2
	rx = x + j
	ry = y + j
	rz = (z+1) * j
	return x, y, rz
end

function anim()
	set_mesh(obj.name,v)
end

add_circle(8,2)
add_circle(8,2)

--every_frame(anim)

]]--
