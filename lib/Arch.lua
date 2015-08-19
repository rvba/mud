-- Arch

require "Class"

local Arch = {}


function Arch:print()
	print("Arch!")
end

function Arch:new()

	local arch = Arch
	setproto(arch,self,"arch")
	return arch
end

local _Arch = Arch
return _Arch

	
