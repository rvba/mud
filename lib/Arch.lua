-- Arch

require "Class"

local Arch = {}
local Seed = {}
local Cell = {}
local Grid = {}

-- DIRECTIONS

local function get_random_direction()
	dirs = { "N", "NE", "E", "SE", "S", "SW", "W", "NW"}
	return dirs[math.random(1,8)]
end

local function get_direction_scalar(dir)

	p = 1 
	dirs = { "N", "NE", "E", "SE", "S", "SW", "W", "NW"}
	for _,v in pairs(dirs) do
		if v == dir then return p
		else p = p + 1
		end
	end
end

local function get_direction_tuple( dir)
	val = {{0,1}, {1,1,},{1,0},{1,-1},{0,-1},{-1,-1,},{-1,0},{-1,1},{1,0}}
	return val[get_direction_scalar( dir)]
end

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
				local cell = Cell:new(grid,i,j)
				grid.cells[j][i] = cell
			end
		end

	return grid
end

function Grid:build()

	for j = 1, self.y do
		for i = 1, self.x do
				cell = self.cells[j][i]
				x = cell.x
				y = cell.y
				local arch = self.arch
				arch:add_vertex(x,y,0)
			end
		end
end

function Grid:get_cell(x,y)
	if x > self.x or x < 1 or y > self.y or y < 1 then
		return nil
	else
		return self.cells[y][x]
	end
end


-- CELL

function Cell:new(grid,x,y)

	local cell = {}
	self.__index = self
	setmetatable(cell,self)
	cell.x = x
	cell.y = y
	cell.grid = grid
	return cell
end

function Cell:print()
	print("cell", self, "x", self.x, "y", self.y)
end

function Cell:get_close( dir)

end

function Cell:get_close(dir)
	
	tuple = get_direction_tuple( dir)
	print("tuple", tuple)
	for k,v in pairs(tuple) do print(k,v) end
	print("cell")
	self:print()
	a = tuple[1]
	b = tuple[2]
	print("a",a,"b",b)
	--x = self.x + tuple[1]
	--y = self.y + tuple[2]
	print("self.x",self.x)
	print("self.y",self.y)
	print("get close")
	close = self.grid:get_cell(x,y)
	if close then
		print("grow!")
		cell = Cell:new(x,y)
		cell:print()
	else
		print("Can't grow")
	end
end


-- SEED

function Seed:new(grid,x,y)

	local seed = Seed
	self.__index = self
	setmetatable(seed,self)
	seed.x = x
	seed.y = y
	seed.dir = get_random_direction()
	seed.cells = {}
	print("new seed")
	table.insert(seed.cells, grid:get_cell( x, y))
	seed.arch = arch
	return seed
end

function Seed:grow()

	print(self.dir)
	for _,cell in pairs( self.cells) do
		print("growing")
		cell:print()
		new = cell:get_close(self.dir)
	end
end

-- ARCH

function Arch:print()
	print("Arch")
	grid  = self.grid
	cells = grid.cells
	for j=1,grid.y do
		for i=1,grid.x do
				print( "point ", 
					"[", cells[j][i].x,
					cells[j][i].y,
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
		v:grow()
	end
end

function Arch:add_seed(x,y)

	seed = Seed:new(self.grid, x, y)
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

	
