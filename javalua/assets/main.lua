
-- game data -------------------------------------------------------------------

-- texture list
g_tex ={}

--sprite collector
g_spc= {}

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

	Lsys.ScriptFile(1, "main.lua")			-- setup the lua script file

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

	g_tex = Ltex.New( "tex/field_tile_0001.png")




	-- create the sprite collector
	g_spc = Lspc.New(g_tex)




	local x
	local y

	local centerX = 300
	local centerY = 0
	local tileCnt = 120

	local r
	local g
	local b
	local a


	for i=1, tileCnt, 1 do
		for j=1, tileCnt, 1 do
			x = ((j-1)*58) - ((i-1)*58) + centerX
			y = ((j-1)*29) + ((i-1)*29) + centerY

	
			r = Lutil.Rand(128, 255)
			g = Lutil.Rand(128, 255)
			b = Lutil.Rand(128, 255)
			a = Lutil.Rand(129, 255)

			r = Lutil.ToHexaByte(r)
			g = Lutil.ToHexaByte(g)
			b = Lutil.ToHexaByte(b)
			a = Lutil.ToHexaByte(a)

			local color = "0X" .. a .. r .. g .. b


			-- gather sprite rectangle
			Lspc.AddRect(g_spc
						, 6  , 34	-- left, top
						, 122,101	-- right, bottom
						, x, y		-- draw positon of screen
						, 1, 1		-- scale
						, 0, 0		-- rotation center
						, 0			-- rotation angle
						, color		-- default is "0xFFFFFFFF"
						)

		end
	end


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

	-- test for performance


	Lspc.Draw(g_spc)


--[[
	local x
	local y

	local centerX = 300
	local centerY = 0
	local tileCnt = 120

	for i=1, tileCnt, 1 do
		for j=1, tileCnt, 1 do
			x = ((j-1)*58) - ((i-1)*58) + centerX
			y = ((j-1)*29) + ((i-1)*29) + centerY

			Ltex.Draw(g_spc, x, y)

		end
	end
--]]


	return 0
end -- Lua_Render
