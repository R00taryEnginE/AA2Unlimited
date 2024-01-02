local _M = {}

local facetoggles = { tears = "00_O_namida", dimeyes = "00_O_mehi", mouthjuice = "A00_O_kutisiru", tonguejuice = "A00_O_sitasiru" }
local facekeys = { eye = { "m_eye" }, eyeopen = { "m_eyeOpenMax", "m_eyeOpenMin" }, eyebrow = { "m_eyebrow" }, mouth = { "m_mouth" }, mouthopen = { "m_mouthOpenMin", "m_mouthOpenMax" }, eyetracking = { "m_eyeTracking" } }
local skelkeys = { pose = "m_poseNumber", frame = "m_animFrame", skelname = "m_name" }
local charkeys = { clothtype = "m_currClothes", clothstate = "m_clothState", materialslot = "m_materialSlot" }
local structfuncs = { destroy = "Destroy", unload = "Unload", update = "Update", spawn = "Spawn" }
local poserfuncs = { getslider = "GetSlider", sliders = "Sliders", override = "Override", props = "Props", loadcloth = "LoadCloth", quatsliderslerp = "QuatSliderSlerp", quatslerp = "QuatSlerp", quat2euler = "quat2euler", euler2quat = "euler2quat" }

function _M.wrap(entity, entmgr)
	log.spam("Poser: Wrapping %s", entity)
	local mt = getmetatable(entity)
	if not entity or not mt then return end
	local ischaracter
	local isprop
	if mt.__name == "ExtClass::CharacterStruct" then
		ischaracter = true
	elseif  mt.__name == "ExtClass::XXFile" then
		isprop = true
	end
	log.spam("Poser: Is character: %s", ischaracter)
	if not (ischaracter or isprop) then
		log.error("Poser: Cannot wrap unknown struct %s", entity)
		return
	end
	-- TODO: maybe auto-set entmgr depending on mt, too. introduces *mgr.lua circular dep tho
	
	local struct = entity
	local poser = ischaracter and GetPoserCharacter(struct) or GetPoserProp(struct)
	local name = ischaracter and sjis_to_utf8(string.format("%s %s", struct.m_charData.m_forename, struct.m_charData.m_surname)) or struct.m_name
	local cache = {}
	local wrapper = {}
	
	local charamt
	charamt = {
		__index = function(t, k)
			local cached = cache[k]
			if cached ~= nil then return cached end
			if ischaracter then
				if facekeys[k] then
					return struct.m_xxFace[facekeys[k][1]]
				end
				if skelkeys[k] then
					return struct.m_xxSkeleton[skelkeys[k]]
				end
				if charkeys[k] then
					return struct[charkeys[k]]
				end
			end
			if structfuncs[k] then
				return function (self, ...) return struct[structfuncs[k]](struct, ...) end
			end
			if poserfuncs[k] then
				return function (self, ...) return poser[poserfuncs[k]](poser, ...) end
			end
			if entmgr and entmgr[k] then
				return function (self, ...) return entmgr[k](wrapper, ...) end
			end
			return charamt[k]
		end,
		
		__newindex = function(t, k, v)
			if ischaracter then
				if facekeys[k] then
					local face = struct.m_xxFace
					rawset(cache, k, v)
					if k == "eyebrow" then
						local eyebrow = face.m_eyebrow
						local offset = eyebrow % 7
						v = eyebrow - offset + v % 7
					end
					for _,prop in ipairs(facekeys[k]) do
						face[prop] = v
					end
					-- character:update_face()
				elseif facetoggles[k] then
					rawset(cache, k, v)
					poser:SetHidden(facetoggles[k], v)
				elseif skelkeys[k] then
					local skel = struct.m_xxSkeleton
					skel[skelkeys[k]] = v
				elseif charkeys[k] then
					struct[charkeys[k]] = v
				else
					rawset(cache, k, v)
				end
			end
		end
	}
	
	rawset(wrapper, "ischaracter", ischaracter)
	rawset(wrapper, "isprop", isprop)
	rawset(wrapper, "struct", struct)
	rawset(wrapper, "poser", poser)
	rawset(wrapper, "name", name)
	rawset(wrapper, "cache", cache)
	rawset(wrapper, "boneselection", {})
	setmetatable(wrapper, charamt)

	if ischaracter then
		function charamt.setclip(character, clip)
			character.pose = clip
			character.frame = 9000
		end

		function charamt.context_name()
			log.spam("chr is %s", name)
			return name .. '@' .. wrapper.pose .. '@' .. (wrapper.xa or '<DEFAULT>')
		end
	end
	
	return wrapper
end

return _M
