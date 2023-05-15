local _M = {}

local fileutils = require "poser.fileutils"
local signals = require "poser.signals"
local charamgr = require "poser.charamgr"
local proxy = require "poser.proxy"

local propschanged = signals.signal()

-- XX objects are kept in bunch of global 'xxlists' of mostly unknown layout.
-- both load and unload needs to know this list. We use only the topmost
-- list which is always shown.
local xxlist
local roomptr

local loaded = {}


-- byte 1 of m_meshFlags denotes some sort of lightning type or slot. loaded props
-- dont have it set, so we have to fix it manually.
--
-- 2 is value used by most of character frames, so we force that value for props as well.
-- Note that character (and its xl) must be loaded for that index to be populated.
--
-- 4 is used by actual 3d rooms when loaded by a game, but that indice disappears
-- when the room is unloaded. At this time, we can't populate our own lightning indices,
-- only piggyback on existing ones. Assigning invalid indices seems to have no-op effect,
-- nothing crashes, just dark light. Suggesting this might be indice either into constant
-- uniform of a fragment shader, or a shader program applied as such.
local function walk_fixlight(f)
	for i=0,f.m_nChildren-1 do
		local c = f:m_children(i)
		c.m_meshFlagLightData = 2
		walk_fixlight(c)
	end
end

function on.end_h()
	roomptr = nil
end

local function loadxx(directory, file)
	-- If loading a room, nuke the one already present
	if roomptr and file:match("^MP_.*") then
		-- XX todo, perhaps make it a separate button
		local orig_room = cast("ExtClass::XXFile", peek_dword(roomptr))
		if orig_room then
			log.spam("unloading previous room %s", orig_room)
			orig_room:Unload(xxlist)
		end
		poke_dword(roomptr, 0)
	end
	local newprop = LoadXX(xxlist, directory .. ".pp", file .. ".xx",0) or nil
	if not newprop then return end
	log.spam("prop struct %s", getmetatable(newprop).__name)
	walk_fixlight(newprop.m_root)
	local currentCharacter = charamgr.currentcharacter();
	if (currentCharacter ~= nil) then
		local character = GetCharInstData(currentCharacter.struct.m_seat)
		if (character ~= nil) then
		character:AddShadows(newprop);
		character:CastShadows(newprop);
		end
	end
	table.insert(loaded, proxy.wrap(newprop))
	log.spam("loaded xx %s at index %d = %s", file, #loaded, newprop)
	propschanged(#loaded)
end

_M.props = loaded
_M.propschanged = propschanged

function _M.loadprop(path)
	local directory, filename, extension = fileutils.splitfilepath(path)
	local directoryname = string.match(directory, ".*\\(.+)\\")
	log.spam("loadprop( %s )\ndirectory: %s\nfilename: %s\nextension: %s", path, directory, filename, extension)
	if directory and filename and extension == "xx" then
		log.spam("loading item %s", filename)
		loadxx(directory, filename)
	end
end

function _M.unloadprop(index)
	local prop = loaded[tonumber(index)]
	if prop then
		log.spam("unloading prop %s", prop.name)
		prop:unload(xxlist)
		table.remove(loaded, index)
		propschanged()
	end
end

local orig_bytes
local load_room_fn = 0x93BB0

function _M.init()
	if exe_type == "edit" then
		xxlist = GameBase + 0x00353290
		return
	else
		xxlist = GameBase + 0x00376298
	end

	orig_bytes = g_hook_func(load_room_fn, 6, 3, function(orig, this, a,b,c)
		roomptr = a + 16
		return proc_invoke(orig, this, a, b, c)
	end)
	log("hooked room loading")
end

function _M.cleanup()
	if orig_bytes then
		g_poke(load_room_fn, orig_bytes)
	end
end

return _M
