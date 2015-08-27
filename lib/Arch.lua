-- Arch

require "Class"

-- TABLES

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
	val = {{0,1}, {1,1,}, {1,0}, {1,-1}, {0,-1}, {-1,-1,}, {-1,0}, {-1,1}, {1,0}}
	return val[get_direction_scalar( dir)]
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

function Arch:grow()

	i = 1
	for _, seed in pairs(self.seeds.seeds) do
		i = i + 1
		seed:grow()
	end
end

function Arch:add_seed(x,y)

	seed = Seed:new(self.grid, x, y)
	self.seeds.seeds[self.seeds.count] = seed
	self.seeds.count = self.seeds.count + 1
	
	return seed
end


function Arch:new()

	local arch = Arch
	setproto( arch,self,"arch")
	self.seeds = {}
	self.seeds.seeds = {}
	self.seeds.count = 0
	self.tempo = 10
	return arch
end

function Arch:update()

	for i = 1, self.tempo do
		print("------------ update arch")
		for _, seed in pairs( self.seeds.seeds) do
			seed:update()
		end
	end
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

-- SEED

function Seed:new(grid,x,y)

	local seed = Seed
	self.__index = self
	setmetatable(seed,self)
	seed.x = x
	seed.y = y
	seed.dir = get_random_direction()
	seed.cells = {}
	seed.cells_built = {}
	local cell = grid:get_cell( x, y)
	cell:build()
	table.insert(seed.cells, cell)
	seed.arch = arch
	seed.power = 10
	return seed
end

function Seed:grow()

	local new_cells = {}

	for _,cell in pairs( self.cells) do
		new = cell:get_close(self.dir)
		if new then
			if not new.built then
				print("growing from")
				cell:print()
				print("to")
				new:print()
				new:build()
				self.power = self.power - 1
				table.insert(new_cells, new)
				table.insert(self.cells_built, cell)
			end
		end
	end

	self.cells = new_cells
end

function Seed:update()
	if self.power > 0 then
		self:grow()
	end
end

-- CELL

function Cell:new(grid,x,y)

	local cell = {}
	self.__index = self
	setmetatable(cell,self)
	cell.x = x
	cell.y = y
	cell.d = .5
	cell.grid = grid
	cell.built = false
	return cell
end

function Cell:print()
	print("cell", self, "x", self.x, "y", self.y, "built", self.built)
end

function Cell:get_close(dir)
	
	tuple = get_direction_tuple( dir)
	x = self.x + tuple[1]
	y = self.y + tuple[2]
	close = self.grid:get_cell(x,y)
	if close then
		return Cell:new(self.grid,x,y)
	else
		print("Can't grow")
		return nil
	end
end

function Cell:build(x,y,d)

	if not cell.built then
		x = self.x
		y = self.y
		d = self.d
		arch = self.grid.arch
		a = arch:add_vertex(x-d,y-d,0)
		b = arch:add_vertex(x+d,y-d,0)
		c = arch:add_vertex(x+d,y+d,0)
		d = arch:add_vertex(x-d,y+d,0)
		arch:add_edge(a,b)
		arch:add_edge(b,c)
		arch:add_edge(c,d)
		arch:add_edge(d,a)
		self.built = true
	end
end

-- RETURN

local _Arch = Arch
return _Arch

	
