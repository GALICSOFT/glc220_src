
-- game data -------------------------------------------------------------------

-- texture list
g_tex ={}
g_ui  ={}


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

	local tx_list = nil

	-- ui 텍스처 생성
	tx_list =
	{
		{ s= "ui_bg.png"		, x=   0, y=   0},
		{ s= "ui_title.png"		, x= 120, y= 100},
		{ s= "ui_start.png"		, x= 275, y= 250},
		{ s= "ui_end.png"		, x= 290, y= 320},
		{ s= "ui_score.png"		, x=   0, y=   0},
		{ s= "ui_continue.png"	, x=   0, y=   0},
		{ s= "ui_rock.png"		, x=   0, y=   0},
		{ s= "ui_scissor.png"	, x=   0, y=   0},
		{ s= "ui_paper.png"		, x=   0, y=   0},
	}


	for i=1, #tx_list do

		local src = tx_list[i]

		g_ui[i] =
		{
			  t = Ltex.New( "tex/".. src.s )
			, x = src.x
			, y = src.y
		}
	end


	for i=5, #g_ui do
		g_ui[i].x = (i - 5) * 100
		g_ui[i].y = 340
	end


	-- game play texture
	tx_list = nil
	tx_list =
	{
		{ s= "img_l_r.png"	, x=   0, y=   0},
		{ s= "img_l_s.png"	, x=   0, y=   0},
		{ s= "img_l_p.png"	, x=   0, y=   0},
		{ s= "img_r_r.png"	, x=   0, y=   0},
		{ s= "img_r_s.png"	, x=   0, y=   0},
		{ s= "img_r_p.png"	, x=   0, y=   0},
	}

	
	for i=1, #tx_list do
		local src = tx_list[i]

		g_tex[i] =
		{
			  t = Ltex.New( "tex/".. src.s )
			, x = 420
			, y = 220 
		}
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

	local st_tx

	for i=1, #g_ui do
		DrawTex(g_ui[i])
	end

	for i=1, #g_tex do
		DrawTex(g_tex[i])
	end

	return 0
end -- Lua_Render



-- User define function ... ----------------------------------------------------
function PrintLn(str)
	print(str .. "\n")
end



function DrawTex(tex)
	Ltex.Draw(tex.t, tex.x, tex.y)
end


function DrawRect(tex, pos, rc)
	Ltex.Draw(tex
				, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h	-- rect
				, pos.x, pos.y						-- position
				)
end


