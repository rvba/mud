-- Arch

require "Class"

local Arch = {}
local Seed = {}


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

function Arch:add_square(x,y,d)
	a = self:add_vertex(x-d,y-d,0)
	b = self:add_vertex(x+d,y-d,0)
	c = self:add_vertex(x+d,y+d,0)
	d = self:add_vertex(x-d,y+d,0)
	self:add_edge(a,b)
	self:add_edge(b,c)
	self:add_edge(c,d)
	self:add_edge(d,a)
end

function Seed:new(x,y)
	local seed = Seed
	seed.x = x
	seed.y = y
	return seed
end

function Arch:add_seed(x,y)
	seeds = self.seeds
	seed = Seed:new()
	seeds[seeds.count] = seed
	seeds.count = seeds.count + 1
	seed.x = x
	seed.y = y
	
	self:add_square(x,y,.1)
	return seed
end

function Arch:new()

	local arch = Arch
	setproto(arch,self,"arch")
	self.seeds = {}
	self.seeds.count = 0
	return arch
end


local _Arch = Arch
return _Arch

	
