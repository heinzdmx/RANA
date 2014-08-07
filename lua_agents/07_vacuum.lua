posX = 0
posY = 0
ID = 0
macroF = 0
timeRes = 0
envWidth = 0
envHeight = 0


-- Init of the lua frog, function called upon initilization of the LUA auton:
function initVacuum(x, y, id, macroFactor, timeResolution)

	posX = x
	posY = y
	ID = id
	macroF = macroFactor
	timeRes = timeResolution

	envWidth, envHeight = l_getEnvironmentSize()

	l_debug("Vacuumer with ID: " .. id .. " has been initialized")

end

-- Event Handling:
function handleEvent(origX, origY, origID, origDesc, origTable)
	--make a response:

		return 0,0,0,"null"
end	

--Determine whether or not this Auton will initiate an event.
function initiateEvent()

	newPosX = posX + l_getMersenneInteger(0,3)-1;
	newPosY = posY + l_getMersenneInteger(0,3)-1;

	if newPosX > envWidth then
		newPosX = 0
	end

	if newPosX < 0 then
		newPosX = envWidth
	end

	if newPosY < 0 then
		newPosY = envHeight
	end

	if newPosY > envHeight then
		newPosY = 0
	end

	--l_debug(newPosX..":"..newPosY)
 	move(newPosX, newPosY)

	r,g,b = l_checkMap(posX,posY)

	if r == 255 then
		l_modifyMap(posX,posY,0,0,0)
	end
	

	return 0,0,0,"null"
end


function getSyncData()
	return posX, posY
end

function simDone()
	l_debug("Agent #: " .. ID .. " is done\n")
end

--function to change position:
function move(newPosX, newPosY)

	--l_debug("moving from X"..posX..", Y"..posY)
	l_updatePosition(posX, posY, newPosX, newPosY,ID)
	posX = newPosX
	posY = newPosY

end

function serializeTbl(val, name, depth)
	--skipnewlines = skipnewlines or false
	depth = depth or 0
	local tbl = string.rep("", depth)
	if name then 
		tbl = tbl .. name .. " = "
		--else tbl = tbl .. "systbl="
	end	
	if type(val) == "table" then
		tbl = tbl .. "{"
		local i = 1
		for k, v in pairs(val) do
			if i ~= 1 then
				tbl = tbl .. ","
			end	
			tbl = tbl .. serializeTbl(v,k, depth +1) 
			i = i + 1;
		end
		tbl = tbl .. string.rep(" ", depth) ..  "}"
	elseif type(val) == "number" then
		tbl = tbl .. tostring(val) 
	elseif type(val) == "string" then
		tbl = tbl .. string.format("%q", val)
	else
		tbl = tbl .. "[datatype not serializable:".. type(val) .. "]"
	end

	return tbl
end
