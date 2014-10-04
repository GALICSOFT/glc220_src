
-- rendering and system data ---------------------------------------------------

-- Window System
g_window ={
			x = -1, y = -1						-- center position -1, -1
			, w = 800, h = 480					-- dc size 800 * 480
			, cls ="GALICSOFT INC"				-- class name
			, full=0							-- screen window mode
			}



-- setup the game system -------------------------------------------------------
function Lua_Create()

	Lsys.ScriptFile(1, "main.lua")				-- setup the lua script file

	Lsys.SetClearColor("0xFF006699")			-- clear color A, R, G, B
	Lsys.ShowCursor(1)							-- show? mouse cursor
	Lsys.ShowState(1)							-- show? system state
	Lsys.ConsoleSet(0)							-- console mode for d3d

	Lsys.CreateWindow(g_window.x, g_window.y	-- window position for pc
					, g_window.w, g_window.h	-- window dc size
					, g_window.cls				-- window class and title
					, g_window.full				-- full mode for d3d
					)

	return 0
end -- Lua_Create




-- Initialize ... --------------------------------------------------------------
function Lua_Init()


	return 0
end -- Lua_Init




-- Release ... -----------------------------------------------------------------
function Lua_Destroy()

	return 0
end -- Lua_Destroy




-- Update data ... -------------------------------------------------------------
function Lua_FrameMove()

	return 0
end -- Lua_FrameMove




-- Rendering ... ---------------------------------------------------------------
function Lua_Render()

	return 0
end -- Lua_Render


