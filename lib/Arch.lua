-- Arch

require "Class"

local Arch = {}
local Seed = {}
local Grid = {}

-- GRID

function Grid:new(arch,x,y)

	local grid = Grid
	grid.x = x
	grid.y = y
	grid.cells = {}
	grid.arch = arch
	for j = 1, y do
		grid.cells[j] = {}
		for i = 1, x do
				grid.cells[j][i] = {}
				grid.cells[j][i].x = i
				grid.cells[j][i].y = j
			end
		end

	return grid
end

function Grid:build()

	for j = 1, self.y do
		for i = 1, self.x do
				x = self.cells[j][i].x
				y = self.cells[j][i].y
				local arch = self.arch
				arch:add_vertex(x,y,0)
			end
		end
end

-- SEED

function Seed:new(grid,x,y)

	local seed = Seed
	self.__index = self
	setmetatable(seed,self)
	seed.x = x
	seed.y = y
	seed.arch = arch
	return seed
end

function Seed:grow()

end

-- ARCH

function Arch:print()
	print("Arch")
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

	self.grid = Grid:new(self,x,y)
	self.grid:build()
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

function Arch:grow()

	i = 1
	for _,v in pairs(self.seeds.seeds) do
		i = i + 1
		v.grow()
	end
end

function Arch:add_seed(x,y)

	seed = Seed:new(self,x,y)
	self.seeds.seeds[self.seeds.count] = seed
	self.seeds.count = self.seeds.count + 1
	
	self:add_square(x,y,.1)
	return seed
end


function Arch:new()

	local arch = Arch
	setproto(arch,self,"arch")
	self.seeds = {}
	self.seeds.seeds = {}
	self.seeds.count = 0
	return arch
end


local _Arch = Arch
return _Arch

	
