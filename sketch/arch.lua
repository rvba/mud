-- arch
local Arch = require "Arch"
local arch = Arch:new()
arch:make_grid(5,5)
arch:build_grid()
arch:add_seed(2,2)
arch:build_object()
