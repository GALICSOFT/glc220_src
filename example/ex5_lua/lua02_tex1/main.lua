
-- game data -------------------------------------------------------------------

g_tex1		= nil								-- texture1 instance
g_tex2		= nil								-- texture2 instance


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

	-- texture create
	g_tex1 = Ltex.New("tex/bg512x256.tga", 16, "0xFF00FFFF")	-- name, 16bit, color key
	PrintLn("texture1 index: " .. g_tex1 .. "-----------------------------")

	g_tex2 = Ltex.New("tex/character02.png")					-- name	==> 16bit, color key=0x00FFFFFF
	PrintLn("texture1 index: " .. g_tex2 .. "-----------------------------")

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

	-- draw texture
	Ltex.Draw(g_tex1
				, 40, 50	-- position
			)


	-- draw with rect and position
	local pos_x = 400
	local pos_y = 100
	local rc_x = 289
	local rc_y = 201
	local rc_w = 104
	local rc_h = 148
	Ltex.Draw(g_tex2
					, rc_x, rc_y, rc_x+rc_w, rc_y+rc_h	-- rect
					, pos_x, pos_y						-- position
				)


	-- use the user defined function
	local pos= {x=500, y=200}
	local rc  ={x=104, y=201, w=92, h=152}

	DrawRect(g_tex2, pos, rc)

	return 0
end -- Lua_Render



-- User define function ... ----------------------------------------------------
function PrintLn(str)
	print(str .. "\n")
end


function DrawRect(tex, pos, rc)
	Ltex.Draw(tex
				, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h	-- rect
				, pos.x, pos.y						-- position
				)
end


