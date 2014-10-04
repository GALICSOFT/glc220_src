
-- game data -------------------------------------------------------------------

-- Key event
KEY_DOWN	= 1
KEY_UP		= 2
KEY_PRESS	= 3


-- Window System
g_window ={
			x = -1, y = -1						-- center position -1, -1
			, w = 800, h = 480					-- dc size 800 * 480
			, cls ="GALICSOFT INC"				-- class name
			, full=0							-- screen window mode
			}

g_sound		= {}								-- sound instance

g_font		= nil								-- font
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

	g_font = Lfont.New("default", 16)
	Lfont.Setup(g_font, "Try to click the l, r - button or touchs", 10, 130, "0xFFFFFF00")


	g_sound[1] = Lsmd.New("groove_mono.wav")
	g_sound[2] = Lsmd.New("snd2.wav")
	g_sound[3] = Lsmd.New("snd3.wav")

	Lsmd.Play(g_sound[1], -1)

	return 0
end -- Lua_Init



-- Release ... -----------------------------------------------------------------
function Lua_Destroy()

	return 0
end -- Lua_Destroy




-- Update data ... -------------------------------------------------------------
function Lua_FrameMove()

	btn_l, btn_r = Lin.MouseEvnt()

	if 1 == btn_l then
		Lsmd.Play(g_sound[2])
	end


	if 1 == btn_r then
		Lsmd.Play(g_sound[3])
	end

	return 0
end -- Lua_FrameMove



-- Rendering ... ---------------------------------------------------------------
function Lua_Render()

	Lfont.Draw(g_font)
	return 0
end -- Lua_Render

