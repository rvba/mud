--require "math"
local P = require "Primitive"
local S = stone

--local obj = P.Cube:new(1,1,1)
local obj = P.Mastaba:new("a",10,10,.1,20)

--obj:add_modifier_rotation(0,0,.5,45)
obj:build()
