-- arch
local Arch = require "Arch"
local arch = Arch:new()
arch:make_grid(5,5)
arch:add_seed(2,2)
arch:grow()
arch:build_object()
