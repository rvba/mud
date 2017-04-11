local P = require "Primitive"
local M = mat4

-- -------------------------------
-- CUBE
-- -------------------------------
--cube = P.Cube:new(1,1,1.5)
--cube:build()

-- -------------------------------
-- MASTABA
-- -------------------------------

-- New
mastaba = P.Mastaba:new(10,10,.1,20)
mastaba2 = mastaba:copy()

-- Matrices
mat = M.new()
mat:identity()
print(mat)

translate = M.new()
translate:identity()

--[[
translate:set(
	1,0,0,0,
	0,1,0,1,
	0,0,1,1,
	10,0,0,1
)
]]--

translate:translate(10,0,0)

rotate = M.new()
rotate:identity()
rotate:rotate('Z', 45)

mat:multiply(translate)
mat:multiply(rotate)

-- In reverse order
--
--mat:multiply(rotate)
--mat:multiply(translate)

mastaba:add_modifier_matrix(mat)

-- Build
mastaba:build()


