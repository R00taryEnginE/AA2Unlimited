--@INFO AAU char editor extension

local _M = {}
local eyeselect = require 'edit.eyeselect'
local sliders = require 'edit.sliders'
local snowflake = require 'edit.snowflake'

local opts = {
	{ "face", 1, "Generate face on save: %b" },
	{ "roster", 1, "Generate roster on save: %b"},
	{ "eyeselect", 1, "Preview eye textures: %b" },
}



function _M.update_ui(eyeonly)
	local base = GetPropW(g_peek_dword(0x353180), GameBase + 0x3100A4)
	local function run(addr,off, ...)
		local val = peek_dword(base+off)
		if (addr == 0x2AD20) then
			invoke_hair_update(GameBase + addr, val)	--	this special snowflake doesn't push arguments to the stack
		else
			proc_invoke(GameBase + addr, nil, val, ...)
		end
	end
	run(0x23640,160) -- updates eyes
	if (eyeonly) then return end

	run(0x1D5B0,128) -- slow

	run(0x1EFC0,136)
	run(0x20E10,144,0) -- updates most sliders
	
	--run(0x22360,152)
	

	run(0x24E20,168,0) -- Eye Color
	-- run(0x25D50,176,0) -- Eye Brows -- disabled because it messes the Hair Color tab
	run(0x26FC0,184) -- Face Detail
	run(0x28350,192) -- semi-slow?
	run(0x28AA0,192)

	run(0x2AD20,200) -- Hair Color
	--run(0x2BC30,208) -- Character

	run(0x2D510,216) -- Personality
	run(0x2DB00,216) -- pose?
	run(0x2F730,224) -- pose?
end

function on.update_edit_gui()
	_M.update_ui()
end

function on.open_help_page(tabName)
	local url = "https://github.com/aa2g/AA2Unlimited/wiki/";
	local help_pages = {
		["General"] = "General",
		["Eye Textures"] = "Eye-Texture",
		["Tan"] = "Tan",
		["Hair"] = "Hair",
		["Mesh Overrides"] = "Mesh-Overrides",
		["Archive Overrides"] = "Archive-Overrides",
		["Archive Redirects"] = "Archive-Redirects",
		["Object Overrides"] = "Object-Overrides",
		["Body"] = "Body",
		["Body Slider"] = "Body-Sliders",
		["Triggers"] = "Triggers",
		["Modules"] = "Modules"
	};


	if (help_pages[tabName] ~= nil) then
		url = url .. help_pages[tabName];
		os.execute('start "" "' .. url .. '"');
	end
end

function _M:load()
	mod_load_config(self, opts)
	if exe_type ~= "edit" then return end
	_M.patcher = patcher()
	local p = _M.patcher
	require "iuplua"

	-- init the submodules
	eyeselect(_M, opts, p)
	sliders(_M, opts, p)
	snowflake(_M, opts, p)
end

function _M:unload()
	_M.patcher:unload()
end

function _M:config()
	mod_edit_config(self, opts, "Editfix options")
end


return _M
