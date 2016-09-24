function _HandleEvent(sourceX, sourceY, originID, description, serialTable)
	
	--local eventTable = {}
	if string.len(serialTable) > 3 then 
		loadstring("_eventTable="..serialTable)()
	else 
		_eventTable = {}
	end

	if HandleEvent ~= nil then 
		HandleEvent{X=sourceX, Y=sourceY, ID=originID, description=description, table=_eventTable}
	
	elseif handleEvent ~= nil then
		handleEvent(sourceX, sourceY, originID, description, _eventTable)
	end

end

function _TakeStep()

	if TakeStep ~= nil then
		TakeStep()
	elseif takeStep ~= nil then
	        takeStep()
	else
	        StepMultiple = 0
	end
end

function _InitializeAgent()

	if InitializeAgent ~= nil then
		InitializeAgent()
	elseif initializeAgent ~= nil then
		initializeAgent()
	else
		say("Agent #: "..ID.. " has been initialized")
	end
end

function _CleanUp()

	if CleanUp ~=nil then
		CleanUp()
	elseif cleanUp ~= nil then
		cleanUp()
	else
		say("Agent #: "..ID.." is done")
	end

end

function _ProcessEventFunction(sourceX, sourceY, posX, posY, time, serialTable)
	
	if string.len(serialTable) > 3 then
		--say(serialTable)
		loadstring("_eventTable="..serialTable)()
	else
		_eventTable = {}
	end

	if ProcessEventFunction == nil then

		if posX == sourceX and posY == sourceY then
			return 1,0
		else
			return .5,0
		end
	else 
		return ProcessEventFunction{sX=sourceX, sY=sourceY, pX=posX, pY=posY, time=time, table=_eventTable}
	end

end

