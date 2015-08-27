-- arch
local Arch = require "Arch"
local arch = Arch:new()
arch:make_grid(30,30)
arch:add_seed(15,15)
arch:grow()
arch:update()
arch:build_object()
