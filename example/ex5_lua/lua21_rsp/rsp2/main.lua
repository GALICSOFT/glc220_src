-------------------------------------------------------------------------------
--
-- Simple Rock+Paper+Scissors
-- Heesung Oh(afewhee@gmail.com)
-- 2012-07-20
--


-- constant -------------------------------------------------------------------

-- define for game phase
GAMEPHASE_BEGIN	= 1
GAMEPHASE_PLAY	= 2
GAMEPHASE_END	= 3
GAMEPHASE_EXIT	= 4

-- Key event
KEY_DOWN		= 1
KEY_UP			= 2
KEY_PRESS		= 3

-- enum of result
PLAY_N			= -1	-- Play None
PLAY_R			= 1		-- rock
PLAY_S			= 2		-- scissor
PLAY_P			= 3		-- paper

-- game data -----------------------------------------------------------------

g_gamePhase		= 0
g_handUsr		= -1	-- user
g_handCom		= -1	-- computer
g_score			= 0		-- game g_score
g_win			= 1		-- win or even

-- mouse
g_mouse_x		= 0
g_mouse_y		= 0
g_mouse_e		= 0


-- rendering data ------------------------------------------------------------

g_window ={x = -1, y = -1, w = 640, h = 480
			, cls ="GALICSOFT INC", full = 0 }

-- texture list
g_tex	={}
g_uibg  ={}
g_uibtn ={}

-- font list
g_fntScore = nil
g_fntMsg   = nil



-- load the lua files ---------------------------------------------------------

Lsys.DoFile("rps_begin.lua")
Lsys.DoFile("rps_play.lua")
Lsys.DoFile("rps_end.lua")



-- setup the game system -------------------------------------------------------
function Lua_Create()

	Lsys.ScriptFile( 1, "main.lua")
	Lsys.SetClearColor("0xFF006699")
	Lsys.ShowCursor(1)
	Lsys.ShowState(0)
	Lsys.ConsoleSet(0)
	Lsys.CreateWindow(g_window.x, g_window.y, g_window.w, g_window.h
						, g_window.cls, g_window.full)

	return 0
end


-- Initialize ... --------------------------------------------------------------
function Lua_Init()


	g_gamePhase	= GAMEPHASE_BEGIN	-- phase


	InitGameData()


	-- game font
	-- score
	g_fntScore = Lfont.New("default", 46)
	Lfont.Setup(g_fntScore, "0", 120, 5, "0xFFFF0000")

	-- message
	g_fntMsg = Lfont.New("default", 16)
	Lfont.Setup(g_fntMsg, "0", 100, 300, "0xFFFF00FF")



	local tx_list = nil

	-- game play texture
	tx_list =
	{
		  {"img_l_r.png", 20 , 60 }
		, {"img_l_s.png", 20 , 60 }
		, {"img_l_p.png", 20 , 60 }
		, {"img_r_r.png", 350, 60 }
		, {"img_r_s.png", 350, 60 }
		, {"img_r_p.png", 350, 60 }
	}

	for i=1, #tx_list do
		g_tex[i] = {n=nil, x=0, y=0 }

		g_tex[i].n = Ltex.New( "tex_play/".. tx_list[i][1] )
		g_tex[i].x = tx_list[i][2]
		g_tex[i].y = tx_list[i][3]
	end

	-- ui bg 持失
	tx_list = nil
	tx_list =
	{
		  {"ui_bg.png"		,   0,   0 }
		, {"ui_title.png"	, 120, 100 }
		, {"ui_score.png"	,  10,  10 }
	}

	for i=1, #tx_list do
		g_uibg[i] = {n=nil, x=0, y=0 }

		g_uibg[i].n = Ltex.New( "tex_ui/".. tx_list[i][1] )
		g_uibg[i].x = tx_list[i][2]
		g_uibg[i].y = tx_list[i][3]
	end


	-- ui button 持失
	tx_list = nil
	tx_list =
	{
		  {"ui_start.png"	, "ui_start_o.png"		,	275, 250 }
		, {"ui_end.png"		, "ui_end_o.png"		,	290, 320 }
		, {"ui_continue.png" , "ui_continue_o.png"	,	290, 320 }
		, {"ui_rock.png"		, "ui_rock_o.png"	,	 20, 330 }
		, {"ui_scissor.png"	, "ui_scissor_o.png"	,	150, 330 }
		, {"ui_paper.png"	, "ui_paper_o.png"		,	280, 330 }
	}

	for i=1, #tx_list do
		g_uibtn[i] = {n=nil, o=nil, x=0, y=0 }

		g_uibtn[i].n = Ltex.New( "tex_ui/".. tx_list[i][1] )
		g_uibtn[i].o = Ltex.New( "tex_ui/".. tx_list[i][2] )
		g_uibtn[i].x = tx_list[i][3]
		g_uibtn[i].y = tx_list[i][4]
	end

	return 0
end



-- Release ... -----------------------------------------------------------------
function Lua_Destroy()

	return 0
end



-- Update data ... -------------------------------------------------------------
function Lua_FrameMove()

	-- update the input
	g_mouse_x, g_mouse_y = Lin.MousePos()
	g_mouse_e = Lin.MouseEvnt()


	if GAMEPHASE_BEGIN == g_gamePhase then
		GameBeginFrameMove()

	elseif GAMEPHASE_PLAY == g_gamePhase then
		GamePlayFrameMove()

	elseif GAMEPHASE_END == g_gamePhase then
		GameEndFrameMove()

	elseif GAMEPHASE_EXIT == g_gamePhase then
		return -1
	end

	return 0
end



-- Rendering ... ---------------------------------------------------------------
function Lua_Render()
	if GAMEPHASE_BEGIN == g_gamePhase then
		GameBeginRender()

	elseif GAMEPHASE_PLAY == g_gamePhase then
		GamePlayRender()

	elseif GAMEPHASE_END == g_gamePhase then
		GameEndRender()
	end

	return 0
end


-- Initialize the game data ---------------------------------------------------

function InitGameData()
	g_handUsr	= -1	-- user
	g_handCom	= -1	-- computer
	g_score		= 0		-- game g_score
	g_win		= 1		-- win or even
end

