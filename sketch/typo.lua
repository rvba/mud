--
local S = require "stone"
local obj = S.new("a")
local a = obj.add_vertex(obj,0,0,0)
local b = obj:add_vertex(1,0,0)
local edge = obj:add_edge(a,b)

local Letters = 
	{
		a = {
				{0,0,0},
				{0,1,0},
				{1,1,0},
				{1,0,0},
				{nil,nil,nil},
				{0,0.5,0},
				{1,0.5,0},
			},
		b = {
				{1,0,0},
				{1,1,0},
			},
		c = {
				{1,0,0},
				{1,1,1},
			},
		nil
	}


local Typo = {}
Typo.object = nil

function Typo:init()
	print("init")	
	self.object = S.new("a")
end

function Typo:build_letter(letter)
	
end

function Typo:print(letter)

	if self.object == nil then
		self:init()
	end

	local list = Letters[letter]

	local prev = nil

	if list ~= nil then

		for k,v in pairs(list) do
			print("element",k,v)
			
			for key,val in pairs(v) do print("point",key,val) end

			if v[1] == nil then
				print("NIIL")
				prev = nil
				next = nil
			else

				next = self.object:add_vertex(v[1],v[2],v[3])
			end

			if prev ~= nil and next ~= nil then
				self.object:add_edge(prev,next)
			end

			prev = next
		end

	
	else
		print("not found")
	end
	
	print(letter)
end

function Typo:new()
	self.__index = self
	return setmetatable({object=nil},Typo)	
end


local test = Typo:new()
test:print("c")
test.object:build_object()















