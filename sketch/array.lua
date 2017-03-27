local P = require "Primitive"
--local M = require "mat4"
local M = mat4

cube = P.Cube:new("aa",1,1,1)
m1 = M.new()
m1:identity()
m1:translate(1.2,0,0)

m2 = M.new()
m2:identity()
m2:rotate('Z',10)

m1:multiply(m2)
cube:add_modifier_array(36,m1)
cube:build()
