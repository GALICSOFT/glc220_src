
-- game data -------------------------------------------------------------------

-- Key event
KEY_DOWN	= 1
KEY_UP		= 2
KEY_PRESS	= 3

-- game state and etc.
TRUE		= 1
FALSE		= 0
OK			= 0
FAIL		= -1


-- rendering and system data

-- Window System
g_window ={
			x = -1, y = -1						-- center position -1, -1
			, w = 800, h = 480					-- dc size 800 * 480
			, cls ="GALICSOFT INC"				-- class name
			, full=0							-- screen window mode
			}

-- input
g_mouse		= {x=0, y=0, e=0}					-- mouse position(0,0), event(0)


g_font		= {}								-- font

g_test_count = 0								-- test count


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

	-- game font
	g_font[1] = Lfont.New("default", 12)
	g_font[2] = Lfont.New("default", 12)
	g_font[3] = Lfont.New("default", 12)

	Lfont.Setup(g_font[1], " "       , 10, 130, "0xFFFFFF00")
	Lfont.Setup(g_font[2], "Back Key", 10,  50, "0xFFFF00FF")
	Lfont.Setup(g_font[3], "Menu Key", 10,  70, "0xFFFF00FF")

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


	-- check the input events
	for i=1, 15 do
		local x
		local y
		local e

		e = Lin.MouseEvnt(i)

		if 0 < e then
			x, y = Lin.MousePos(i)			-- the wheel(event 3) position is saved x on windows system

			local str = string.format("[%d]:: (%d, %d) %d", i, x, y, e)
			Lfont.String(g_font[1], str)
			Lfont.Position(g_font[1], x+30, y+20)
			Lfont.Draw(g_font[1])
		end
	end


	local key_back = Lin.KeyboardOne(8)		-- back key
	local key_menu = Lin.KeyboardOne(123)	-- menu(F12) key



	for i=1, 128 do

		if i ~= 21 and i ~= 64  then

			local st = Lin.KeyboardOne(i)

			if 1 < st then
				print(i, st, "\n")
			end
		end

	end




	Lfont.Draw()
	Lfont.Draw()


	Lfont.String(g_font[2], "back::" .. key_back)
	Lfont.Draw(g_font[2])

	if 0 < key_menu then
		Lfont.Draw(g_font[3])
	end


	return 0
end -- Lua_Render

