--
-- Title:
-- Author:
-- Date:
--


-- constant --------------------------------------------------------------------

-- Key event
KEY_DOWN	= 1
KEY_UP		= 2
KEY_PRESS	= 3

-- game state and etc.
TRUE		= 1
FALSE		= 0
OK			= 0
FAIL		= -1


-- game data -------------------------------------------------------------------


-- rendering and system data ---------------------------------------------------

-- Window System
g_window ={
			x = -1, y = -1						-- center position -1, -1
			, w = 800, h = 480					-- dc size 800 * 480
			, cls ="GALICSOFT INC"				-- class name
			, full=0							-- screen window mode
			}

-- input
g_mouse		= {x=0, y=0, e=0}					-- mouse position(0,0), event(0)

-- font
g_font1		= nil
g_font2		= nil
g_font3		= nil
g_font4		= nil
g_font5		= nil


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
	g_font1 = Lfont.New("default", 12)
	g_font2 = Lfont.New("default", 12)
	g_font3 = Lfont.New("default", 14)
	g_font4 = Lfont.New("default", 12)
	g_font5 = Lfont.New("default", 12)

	Lfont.Setup(g_font1, "¾È³ç", 10, 130, "0xFFFFFF00")
	Lfont.Setup(g_font2, "¾È³ç", 300, 5, "0xFF00FFFF")
	Lfont.Setup(g_font3, "¾È³ç", 10, 30, "0xFFFF00FF")
	Lfont.Setup(g_font4, "Back Key", 10, 50, "0xFFFF00FF")
	Lfont.Setup(g_font5, "Menu Key", 10, 70, "0xFFFF00FF")

	return 0
end -- Lua_Init



-- Release ... -----------------------------------------------------------------
function Lua_Destroy()

	return 0
end -- Lua_Destroy



g_sensorIdx =1

sensor_type =
{
	  "ACCELEROMETER"
	, "MAGNETIC_FIELD"
	, "ORIENTATION"
	, "GYROSCOPE"
	, "LIGHT"
	, "PRESSURE"
	, "TEMPERATURE"
	, "PROXIMITY"
	, "GRAVITY"
	, "LINEAR_ACCELERATION"
	, "ROTATION_VECTOR"
}


g_test_count = 0
-- Update data ... -------------------------------------------------------------
function Lua_FrameMove()

	g_test_count = g_test_count +1

	local cnt = Lin.MouseCount()

	if 5000<g_test_count then
		return -1
	end

	Lfont.String(g_font2, "Touch:: " .. cnt)

	if 1 == cnt then
		Lhw.HapticPlay(50)
	end



	local x
	local y
	local z
	local str


	if 1 == Lin.MouseEvnt() then
		g_sensorIdx = g_sensorIdx + 1

		if #sensor_type < g_sensorIdx then
			g_sensorIdx = 1
		end
	end


	-- default is GRAVITY
	x, y, z = Lhw.Sensor()
	str = string.format("Gravity:: %.1f  %.1f  %.1f", x, y, z)


	-- other sensor values
	---[[
	x, y, z = Lhw.Sensor(sensor_type[ g_sensorIdx ])
	str = string.format("%s:: %.1f  %.1f  %.1f", sensor_type[ g_sensorIdx ], x, y, z)
	--]]



	Lfont.String(g_font3, str)

	return 0
end -- Lua_FrameMove



-- Rendering ... ---------------------------------------------------------------
function Lua_Render()

	-- draw font


	-- update the input

	for i=1, 15 do
		local x
		local y
		local e

		e = Lin.MouseEvnt(i)

		if 0 < e then
			x, y = Lin.MousePos(i)

			local str = string.format("[%d]:: (%d, %d) %d", i, x, y, e)
			Lfont.String(g_font1, str)
			Lfont.Position(g_font1, x, y)
			Lfont.Draw(g_font1)
		end
	end


	Lfont.Draw(g_font2)
	Lfont.Draw(g_font3)

	local key_back = Lin.KeyboardOne(8)		-- back key
	local key_menu = Lin.KeyboardOne(123)	-- menu(F12) key


	--[[
	for i=5, 255 do
		local st = Lin.KeyboardOne(i)

		if 0 < st then
			print(i, st, "\n")
		end
	end
	--]]



	Lfont.String(g_font4, "back::" .. key_back)
	Lfont.Draw(g_font4)

	if 0 < key_menu then
		Lfont.Draw(g_font5)
	end


	return 0
end -- Lua_Render

