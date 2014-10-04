-------------------------------------------------------------------------------
--
-- Simple Rock+Paper+Scissors
-- Heesung Oh(afewhee@gmail.com)
-- 2012-07-20
--



-- constant -------------------------------------------------------------------

-- Key event
KEY_DOWN	= 1
KEY_UP		= 2
KEY_PRESS	= 3

-- enum of result
PLAY_N		= -1	-- Play None
PLAY_R		= 1		-- rock
PLAY_S		= 2		-- scissor
PLAY_P		= 3		-- paper

-- game data -----------------------------------------------------------------

g_handUsr	= -1	-- user
g_handCom	= -1	-- computer
g_score		= 0		-- game g_score
g_win		= 1		-- win or even

-- mouse
g_mouse_x	= 0
g_mouse_y	= 0
g_mouse_e	= 0

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


	InitGameData()


	-- game font
	-- score
	g_fntScore = Lfont.New("default", 46)
	Lfont.Setup(g_fntScore, "0", 120, 5, "0xFFFF0000")

	-- message
	g_fntMsg = Lfont.New("default", 16)
	Lfont.Setup(g_fntMsg, "0", 100, 300, "0xFFFF00FF")


	local tx_list = nil



	-- ui bg 생성
	tx_list = nil
	tx_list =
	{
		  "ui_bg.png"		,   0,   0
		, "ui_score.png"	,  10,  10
		, ""
	}

	local i = 1
	while true do

		if "" == tx_list[3* (i-1) + 1] then
			break
		end

		local st_tx ={n=0, x=0, y=0}

		st_tx.n = Ltex.New( "tex_ui/" .. tx_list[3* (i-1) + 1] )
		st_tx.x = tx_list[3* (i-1) + 2]
		st_tx.y = tx_list[3* (i-1) + 3]

		g_uibg[i] = st_tx

		i = i + 1
	end



	-- game play texture
	tx_list =
	{
		  "img_l_r.png"
		, "img_l_s.png"
		, "img_l_p.png"
		, "img_r_r.png"
		, "img_r_s.png"
		, "img_r_p.png"
	}

	for i=1, #tx_list do

		local st_tx ={n=0, x=420, y=220}
		st_tx.n = Ltex.New( "tex_play/".. tx_list[i] )

		g_tex[i] = st_tx
	end


	g_tex[1].x = 20 ; g_tex[1].y = 60
	g_tex[2].x = 20 ; g_tex[2].y = 60
	g_tex[3].x = 20 ; g_tex[3].y = 60

	g_tex[4].x = 350; g_tex[4].y = 60
	g_tex[5].x = 350; g_tex[5].y = 60
	g_tex[6].x = 350; g_tex[6].y = 60


	
	-- ui button 생성
	tx_list = nil
	tx_list =
	{
		  { "ui_rock.png"		,	 20, 330 }
		, { "ui_scissor.png"	,	150, 330 }
		, { "ui_paper.png"		,	280, 330 }
	}

	for i=1, #tx_list do

		g_uibtn[i] = { n=0, x=0, y=0 }
		
		g_uibtn[i].n = Ltex.New( "tex_ui/".. tx_list[i][1] )
		g_uibtn[i].x = tx_list[i][2]
		g_uibtn[i].y = tx_list[i][3]
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


	local s_buf = nil
	local play = 0


	if 0 < g_win then

		if   30 < g_mouse_x and g_mouse_x <130 and
			330 < g_mouse_y and g_mouse_y <450 then

			if g_mouse_e == KEY_DOWN then
				play = 1

				g_handCom = Lutil.Rand(3)
				g_handUsr = PLAY_R

				s_buf = nil
				s_buf = string.format("[User, Computer]:%d %d\n", g_handUsr, g_handCom)
				print(s_buf)
			end
		end


		if  170 < g_mouse_x and g_mouse_x <260 and
			330 < g_mouse_y and g_mouse_y <450 then

			if g_mouse_e == KEY_DOWN then
				play = 1

				g_handCom = Lutil.Rand(3)
				g_handUsr = 2

				s_buf = nil
				s_buf = string.format("[User, Computer]:%d %d\n", g_handUsr, g_handCom)
				print(s_buf)
			end
		end

		if  290 < g_mouse_x and g_mouse_x <400 and
			330 < g_mouse_y and g_mouse_y <450 then

			if g_mouse_e == KEY_DOWN then
				play = 1

				g_handCom = Lutil.Rand(3)
				g_handUsr = 3

				s_buf = nil
				s_buf = string.format("[User, Computer]:%d %d\n", g_handUsr, g_handCom)
				print(s_buf)
			end
		end


		if 0 < g_handUsr and 0 < play  then
			-- 바위
			if     g_handUsr == PLAY_R then
				if g_handCom == PLAY_R then
					Lfont.String(g_fntMsg, "[바위(나) - 바위(컴)]: 비겼습니다. 다시합니다.")

				elseif g_handCom == PLAY_S then
					g_score = g_score + 1000

					s_buf = nil
					s_buf = string.format("%d", g_score)
					Lfont.String(g_fntScore, s_buf)
					Lfont.String(g_fntMsg, "[바위(나) - 가위(컴)]: 당신이 이겼습니다.")

				else
					g_win = -1
					Lfont.String(g_fntMsg, "[바위(나) - 보(컴)]: 졌군요.")
				end

			-- 가위
			elseif g_handUsr == PLAY_S then

				if g_handCom == PLAY_R then
					g_win = -1
					Lfont.String(g_fntMsg, "[가위(나) - 바위(컴)]: 졌군요.")

				elseif g_handCom == PLAY_S then
					Lfont.String(g_fntMsg, "[가위(나) - 가위(컴)]: 비겼습니다. 다시합니다.")
				else
					g_score = g_score + 1000

					s_buf = nil
					s_buf = string.format("%d", g_score)
					Lfont.String(g_fntScore, s_buf)
					Lfont.String(g_fntMsg, "[가위(나) - 보(컴)]: 당신이 이겼습니다.")
				end

			-- 보
			elseif g_handUsr == PLAY_P then
				if g_handCom == PLAY_R then

					print "[보(나) - 바위(컴)]: 당신이 이겼습니다.\n"
					g_score = g_score + 1000

					s_buf = nil
					s_buf = string.format("%d", g_score)
					Lfont.String(g_fntScore, s_buf)
					Lfont.String(g_fntMsg, "[보(나) - 바위(컴)]: 당신이 이겼습니다.")

				elseif g_handCom == PLAY_S then
					g_win = -1
					Lfont.String(g_fntMsg, "[보(나) - 가위(컴)]: 졌군요.")
				else
					Lfont.String(g_fntMsg, "[보(나) - 보(컴)]: 비겼습니다. 다시합니다.")
				end
			end
		end
	end


	if 0 == g_win and g_mouse_e == KEY_DOWN then
		InitGameData()
		Lfont.String(g_fntMsg, " ")
		Lfont.String(g_fntScore, "0")
	end

	if 0 >	g_win then
		g_win = 0
	end

	return 0
end



-- Rendering ... ---------------------------------------------------------------
function Lua_Render()
	local UI_SCORE     = 2
	local UI_ROCK_S    = 1
	local UI_SCISSOR_S = 2
	local UI_PAPER_S   = 3

	local st_tx

	st_tx = g_uibg[1];				Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)
	st_tx = g_uibg[UI_SCORE];		Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)


	if 0 < g_win then
		st_tx = g_uibtn[UI_ROCK_S];		Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)
		st_tx = g_uibtn[UI_SCISSOR_S];	Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)
		st_tx = g_uibtn[UI_PAPER_S];	Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)
	end

	Lfont.Draw(g_fntScore)


	if 0 > g_handUsr then
		return
	end


	st_tx = g_tex[g_handUsr];		Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)
	st_tx = g_tex[g_handCom+3];		Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)

	Lfont.Draw(g_fntMsg)

	return 0
end


-- Initialize the game data ---------------------------------------------------

function InitGameData()
	g_handUsr	= -1	-- user
	g_handCom	= -1	-- computer
	g_score		= 0		-- game g_score
	g_win		= 1
end
