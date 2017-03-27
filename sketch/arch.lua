-- arch
local Arch = require "Arch"
local arch = Arch:new()
arch:make_grid(4,4)
arch:add_seed(2,2)
arch:grow()
arch:update()
arch:build()
