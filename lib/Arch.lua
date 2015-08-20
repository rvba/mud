-- Arch

require "Class"

local Arch = {}


function Arch:print()
	for j=1,self.y do
		for i=1,self.x do
				print( "point ", 
					"i ", i, "j ", j, "n", i+j,
					"[", self.grid[i+j].x,
					self.grid[i+j].y,
					"]"
					)
			end
		end
end

function Arch:make_grid(x,y)

	self.grid={}
	self.x=x
	self.y=y
	n = 1
	for j=1,y do
		for i=1,x do
				self.grid[n] = {}
				self.grid[n].x = i
				self.grid[n].y = j
				n = n + 1
			end
		end
end

function Arch:build_grid()
	n = 1
	for j=1,self.y do
		for i=1,self.x do
				x = self.grid[n].x
				y = self.grid[n].y
				self:add_vertex(x,y,0)
				n = n + 1
			end
		end
end

function Arch:new()

	local arch = Arch
	setproto(arch,self,"arch")
	return arch
end


local _Arch = Arch
return _Arch

	
