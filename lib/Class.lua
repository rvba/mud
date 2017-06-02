local wrapper = {}
local use_debug = false

-- Wrapper

local function find(table,key)
	local content = table[key]
end

function wrapper:__index(key)

	-- Get self
	_self = rawget(self,"__self__")
	local f = rawget(_self,key)

	-- Object's self functions
	if f then
		if type(f) == "function" then
			return function (me,...)
				return f(me,...)
			end
		else
			return f
		end
	else
		-- Stone's functions
		local proto = rawget(self,"__proto__")
		if use_debug then
			if pcall(proto,find(key)) then
			else
				print(debug.traceback())
			end
		end

		local field = proto and proto[key]

		if type(field) ~= "function" then
			return field
		else
			return function(obj,...)
				if obj == self then
					return field(proto,...)
				else
					return field(obj,...)
				end
			end
		end
	end
	
end

function setproto(o,self,name)
	setmetatable(o,wrapper)
	o.__proto__ = stone.new(name)
	o.stone = o.__proto__
	o.__self__ = self
end


