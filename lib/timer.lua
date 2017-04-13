local _M =  _M or {}

local records = {} 

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

_M.clock = clock
_M.delta = delta
_M.test = test

return _M
