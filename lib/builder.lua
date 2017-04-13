-- builder module
--
--! @file builder.lua
--! @brief builder module
--!
--! frames builder utility


--! @brief builder test function
local function test_builder()
	local a = 1
end

local reverse = require "reverse"
local Frame = require "Frame"

local _M = _M or {} 

local debug = true
local now = os.time()
local delta = 0

local do_plain = 0
local frames = {}
local nodes = {}
local levels = {}

local divisions = 0
local level_count = 0
local resolution = nil

local depth = .2
local thick = .1
local slab = 1

local built = false
local init = false
local last_version = 0


local function get_level(l,d,v)
	local p = {}
	if debug then print("[lua] get level", l) end
	for i=0,d-1 do
		local o = l * d * 3
		p[(i*3)+0] = v[(i*3)+0+o]
		p[(i*3)+1] = v[(i*3)+1+o]
		p[(i*3)+2] = v[(i*3)+2+o]
		if debug then print(i, p[(i*3)+0],p[(i*3)+1],p[(i*3)+2]) end
	end
	return p
end

-- down=level_down, up=level_up, p=pos
local function build_vectors(down,up,p,last)

	local a,b,c,d = nil
	if last then
		a = { down[(p*3)+0], down[(p*3)+1], down[(p*3)+2] }
		b = { down[0], down[1], down[2] }
		c = { up[0], up[1], up[2] }
		d = { up[(p*3)+0], up[(p*3)+1], up[(p*3)+2] }
	else
		a = { down[(p*3)+0], down[(p*3)+1], down[(p*3)+2] }
		b = { down[((p+1)*3)+0], down[((p+1)*3)+1], down[((p+1)*3)+2] }
		c = { up[((p+1)*3)+0], up[((p+1)*3)+1], up[((p+1)*3)+2] }
		d = { up[(p*3)+0], up[(p*3)+1], up[(p*3)+2] }
	end

	if nil then 
	print("a",i,p,a[1],a[2],a[3]) 
	print("b",i,p,b[1],b[2],b[3])
	print("c",i,p,c[1],c[2],c[3])
	print("d",i,p,d[1],d[2],d[3])
	end

	local vO = smath.new(a[1],a[2],a[3])
	local vA = smath.new(b[1],b[2],b[3])
	local vB = smath.new(d[1],d[2],d[3]) -- d
	local vC = smath.new(c[1],c[2],c[3]) -- c


	vA:sub(vO)
	vB:sub(vO)
	vC:sub(vO)

	return vO,vA,vB,vC
end

-- i=level
local function build_frames(i,build)

	local down = levels[i]
	local up = levels[i+1]
	local frames = {}
	local indice = 1

	for p = 0, (divisions - 2) do -- d - 2 * 3 

		local vO, vA, vB, vC = build_vectors(down,up,p,nil)
		local frame = Frame.frame:new(vO, vA, vB, vC)
		frame:construct() 
		frames[indice] = frame
		indice = indice + 1
	end

	-- last frame
	local p = divisions - 1
	local vO, vA, vB, vC = build_vectors(down,up,p,1)
	local frame = Frame.frame:new( vO, vA, vB, vC)
	frame:construct() 
	frames[indice] = frame

	return frames
end

local function update(node)

	local indice = 0 
	local version = node.version
	if version ~= last_version then
		last_version = version

		if debug then print("[builder] new version", version) end

		-- build level rings
		for i = 0, level_count do
			levels[i] = get_level(i,divisions,node.vertices)
		end

		-- update 
		for i = 0, level_count - 1 do

			local _frames = build_frames(i,nil)

			for _,f in ipairs(_frames) do
				local new_frame = f.stone
				local _old_frame = frames[indice]
				local old_frame = _old_frame.stone
				print(old_frame.name)
				local reverse_id = nodes[indice] 

				if reverse_id ~= nil then

					local verse_id = verse.get_object_id(reverse_id)
					local node = reverse.get_node_by_id(verse_id)
					if node then

						for i=0, new_frame.vertex_count - 1 do
							local x,y,z = new_frame:get_vertex_location(i)

							update_mesh(old_frame.name,i,x,y,z);
							old_frame:set_vertex(i,x,y,z)
							node:set_vertex(i,x,y,z)
						end
					else 
						print("[builder] not found")
						print("[builder] reverse_id",reverse_id)
						print("[builder] verse_id", verse_id)
					end
				else 
					print("[builder] no indice")
					print("[builder] indice", indice)
					print("[builder] reverse_id",reverse_id)
				end
				
				indice = indice + 1
			end
		end
	end
end

-- l=levels, d=divisions, v=vertices
local function build_it(v)

	if debug then
	print("[builder.lua] build")
	print("[builder.lua] level count", level_count)
	print("[builder.lua] divisions", divisions)
	end

	local indice = 0 

	-- build level rings
	for i = 0, level_count do
		levels[i] = get_level(i,divisions,v)
	end

	-- build frames between levels
	for i = 0, level_count - 1 do 

		local _frames = build_frames(i,1)

		for _,f in ipairs(_frames) do
			local frame = f

			-- push individual frame
			local _, object = reverse.push(frame.stone)

			-- store 
			frames[indice] = frame
			nodes[indice] = object

			indice = indice + 1
		end
	end

	-- set built
	built = true
end

-- init: extract informations 
local function build_init(node)

		local points = node.vertices
		id_verse = node.id
		id_tag_group = 0
		local tag_group_name = node:get_tag_group_name(0)
		local name = verse.get_tag_name(id_verse,id_tag_group,0)
		local tag_name = verse.get_tag_name(node.id,0,0)

		if debug then 
			print("[builder.lua]",node.name,id_verse) 
			print("[builder.lua] tag_group_name",tag_group_name)
			print("[builder.lua] tag_name",tag_name)
		end

		-- tag found
		if name ~= "not_found" then

			init = true

			-- extract tag values
			local levels_string = string.sub(name,1,3)
			level_count = math.floor(tonumber(levels_string))
			div = string.sub(name,5,7)
			divisions = math.floor(tonumber(div))

			if true then
				print("[builder.lua] Init level count:",level_count,"divisions:",divisions)
			end

			-- build: l=levels, d=divisions, points=vertices
			build_it(points)

			last_version = node.version
		end
end

-- build: main function
local function build(node)

	if not init then
		-- init
		build_init(node)
	else
		if built then
			-- update
			update(node)
		end
	end
end


-- module methods
_M.add_frame = add_frame
_M.add_frame_plain = add_frame_plain
_M.build = build
_M.update = update


return _M
