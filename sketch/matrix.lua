local P = require "Primitive"
--local M = require "mat4"
local M = mat4

t = 1

m1 = M.new()
m1:identity()

m2 = M.new()
m2:identity()

m3 = M.new()
m3:identity()
m3:rotate('Z',10)

m1:translate(t,0,0)
m2:translate(0,t,0)

m1:multiply(m2)
--m1:add(m2)

m3:multiply(m1)
--m3:multiply(m3)
--m3:multiply(m3)
--m3:multiply(m3)
--m3:multiply(m3)

cube = P.Cube:new("a",1,1,1)
cube:add_modifier_matrix(m3)
cube:build()
