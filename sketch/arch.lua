-- arch
local Arch = require "Arch"
local arch = Arch:new()
arch:make_grid(5,5)
arch:build_grid()
arch:print()
arch:build_object()
