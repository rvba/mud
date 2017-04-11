

-- ts_internal_bspline_new
-- if (deg >= n_ctrlp)
-- point count must be at least equal to curve degree

local P = require "Primitive"
local s = P.Spline:new()
s.degree = 4
s.resolution = 15
s:add(1,2,0)
s:add(3,3,0)
s:add(6,0,0)
s:add(9,3,0)
s:add(12,0,0)
s:print()
s:make()
