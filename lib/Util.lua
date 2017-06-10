
local names = {}
local records = {} 

local _M = {}

local function adapt()


	local screen_width = 800
	local screen_height = 500

	-- Host
	local handle = io.popen("hostname")
	local host = handle:read()
	handle.close()

	if host == "thinkpad" then
		screen_width = 1280 / 2
		screen_height = 355 
	end

	-- Reshape
	reshape(screen_width,screen_height)

	-- Move
	local w = win_get()
	if w == 1 then win_move(screen_width,0) -- right top
	elseif w == 2 then win_move(screen_width,screen_height) --right down
	elseif w == 3 then win_move(0,screen_height) --left down
	elseif w == 4 then win_move(0,0)
	end
end

local function clock(name,interval)

	local now = os.time()
	record = records[name]
	if not record then
		records[name] = { start = now, interval = interval }
		return false
	else
		if now - record["start"] >= interval then
			record["start"] = now
			return true
		else
			return false
		end
	end
end

local function delta(name)
	local now = os.time()
	record = records[name]
	if record then
		return now - record["start"] 
	else
		return 0
	end
end

local function test()
	if timer.clock("test",3) then
		print("clock!")
	end
end


local function set_name(name)

	local entry = names[name]
	local count = 0
	if entry == nil then
		names[name] = {name=name,count=1}
		count = 1
	else
		names[name]["count"] = names[name]["count"] + 1
		count = names[name]["count"]
	end
	
	return name .. "-" .. count 
end

local function round(num, numDecimalPlaces)
  local mult = 10^(numDecimalPlaces or 0)
    return math.floor(num * mult + 0.5) / mult
    end

local function reset()
	names = {}
end

_M.set_name = set_name
_M.round = round
_M.clock = clock
_M.delta = delta
_M.test = test
_M.reset = reset
_M.adapt = adapt


return _M


