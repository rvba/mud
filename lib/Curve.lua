local _M = _M or {} 

local function circle(resolution,radius)

	angle = math.pi * 2 / resolution
	a = 0
	pt = nil
	start = nil
	circle = {}

	j = 0
	for i = 1,resolution
	do
		x = math.cos(a) * radius
		y = math.sin(a) * radius
		z = 0

		circle[i] = {x,y,z}

		a = a + angle
		j = j + 3

	end
	return circle
end

_M.Circle = circle

return _M


