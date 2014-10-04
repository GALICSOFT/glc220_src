-------------------------------------------------------------------------------
--
-- hidden catch(spot the difference) puzzle
-- Heesung Oh(afewhee@gmail.com)
-- 2012-07-20
--


-- constant -------------------------------------------------------------------

-- Key event
KEY_DOWN	= 1
KEY_UP		= 2
KEY_PRESS	= 3

-- game state and etc....
TRUE		= 1
FALSE		= 0
FAIL		= -1

-- game data -----------------------------------------------------------------

g_score		= 0					-- game g_score
g_state		= 0					-- none: 0, active: 1, fail: 0
g_stage		= 3
g_time		= 200
g_check		= { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30}

-- rendering and system data --------------------------------------------------

g_window ={x =1, y = 10, w = 800, h = 480
			, cls ="GALICSOFT INC", full = 0 }

-- mouse
g_mouse_x	= 0
g_mouse_y	= 0
g_mouse_e	= 0

-- font list
g_fntScore	= nil
g_fntTime	= nil
g_fntMsg	= nil

-- texture list
g_tex_bg	={}
g_tex_ui	= nil
g_rc_check	={}
g_btn		={}



-- setup the game system -------------------------------------------------------

Lsys.DoFile("util.lua")

function Lua_Create()

	Lsys.ScriptFile( 1, "main.lua")
	Lsys.SetClearColor("0xFF006699")
	Lsys.SetClearColor("0xFFFFFFFF")
	Lsys.ShowCursor(1)
	Lsys.ShowState(0)
	Lsys.ConsoleSet(0)
	Lsys.CreateWindow(g_window.x, g_window.y, g_window.w, g_window.h
						, g_window.cls, g_window.full)

	return 0
end



-- Initialize ... --------------------------------------------------------------
function Lua_Init()


	-- game font
	-- score
	g_fntScore = Lfont.New("default", 24)
	Lfont.Setup(g_fntScore, "Á¡¼ö: 0", 20, 400, "0xFFcc3300")

	-- time
	g_fntTime = Lfont.New("default", 24)
	Lfont.Setup(g_fntTime, "Time: 0", 300, 400, "0xFF0033cc")

	-- message
	g_fntMsg = Lfont.New("default", 32)
	Lfont.Setup(g_fntMsg, "Game Over", 100, 230, "0xFFFF3300")


	---------------------------------------------------------------------------
	-- game textures
	---------------------------------------------------------------------------
	local tmp_lst
	local i; local j


	-- ui texture
	g_tex_ui =  Ltex.New("tex/" .. "ui.png")


	---------------------------------------------------------------------------	
	-- load the back ground texture
	tmp_lst = ReadCsv("data/catch_img.csv")
	for i=1, #tmp_lst do
		local st_data = {tx_l=0, tx_r=0, x=0, y=0, w=0, h=0 }
		
		st_data.tx_l = Ltex.New("tex/" .. tmp_lst[i][1])
		st_data.tx_r = Ltex.New("tex/" .. tmp_lst[i][2])
		st_data.x    = tmp_lst[i][3]
		st_data.y    = tmp_lst[i][4]
		st_data.w    = tmp_lst[i][5]
		st_data.h    = tmp_lst[i][6]

		g_tex_bg[i] = st_data
	end

	tmp_lst = nil


	---------------------------------------------------------------------------	
	-- rect data
	tmp_lst = ReadCsv("data/catch_rect.csv")

	for i= 1, #tmp_lst do

		local st_data = {x=0, y=0, w=0, h=0}

		st_data.x = tmp_lst[i][1]
		st_data.y = tmp_lst[i][2]
		st_data.w = tmp_lst[i][3]
		st_data.h = tmp_lst[i][4]

		g_rc_check[i] = st_data
	end

	tmp_lst = nil


	---------------------------------------------------------------------------
	-- load the button
	tmp_lst = ReadCsv("data/ui_btn.csv")
	for i=1, #tmp_lst do

		local st_data = {x0=0, y0=0, x1=0, y1=0, w=0, h=0 }

		local name   = tmp_lst[i][1]

		st_data.w    = tmp_lst[i][2]
		st_data.h    = tmp_lst[i][3]
		st_data.x0   = tmp_lst[i][4]
		st_data.y0   = tmp_lst[i][5]
		st_data.x1   = tmp_lst[i][6]
		st_data.y1   = tmp_lst[i][7]

		g_btn[name] = st_data
		--print(name .. "\n")
	end

	tmp_lst = nil

	---------------------------------------------------------------------------
	InitGameData()

	return 0
end



-- Release ... -----------------------------------------------------------------
function Lua_Destroy()

	return 0
end



-- Update data ... -------------------------------------------------------------
function Lua_FrameMove()

	if FAIL == g_state then
		return 0
	end

	-- update the input
	g_mouse_x, g_mouse_y = Lin.MousePos()
	g_mouse_e = Lin.MouseEvnt()


	return 0
end



-- Rendering ... ---------------------------------------------------------------
function Lua_Render()

	-- draw bg
	Ltex.Draw(g_tex_bg[g_stage].tx_l,   0, 0)
	Ltex.Draw(g_tex_bg[g_stage].tx_r, 400, 0)

	-- draw check
	DrawCheck(g_tex_ui, g_btn["ui_ch"], g_check, g_rc_check)

	-- draw next, exit button
	DrawButton(g_tex_ui, g_btn["ui_next"], 550, 385)
	DrawButton(g_tex_ui, g_btn["ui_exit"], 670, 385)

	-- draw score, time, message
	Lfont.Draw(g_fntScore)
	Lfont.Draw(g_fntTime)

	if FAIL == g_state then
		Lfont.Draw(g_fntMsg)
	end
	
	return 0
end


-- Initialize the game data ---------------------------------------------------

function InitGameData()
	g_score		= 0		-- game g_score
	g_state		= TRUE

end

