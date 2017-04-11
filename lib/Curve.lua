local _M = _M or {} 

local function circle(resolution,radius)

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

_M.Circle = circle

return _M


