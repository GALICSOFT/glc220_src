

g_Font = {}


function Lua_Create()

	Lsys.ScriptFile( 1, "main.lua")

	Lsys.SetClearColor("0x0")
	Lsys.ShowCursor(1)
	Lsys.ShowState(1)
	Lsys.ConsoleSet(0)

	return Lsys.CreateWindow(-1, -1, 800, 480, "GALICSOFT INC", 0)
end


function Lua_Init()

	local font_list = { "default", "nanum_pen.ttf", "seoul_hangang_l08.ttf"}

	local y = 10
	for i=1, 20 do
		local h = i+9

		g_Font[i] = Lfont.New( font_list[i%3 + 1] , 9+i)

		y = y + h*1.5
		Lfont.Setup(g_Font[i], h .. ":: 이것은 한글입니다 123ABCZ efghijklm", 10, y, "0xFF00FFFF")

	end


	return 0
end


function Lua_Destroy()

	return 0
end


function Lua_FrameMove()
	return 0
end



function Lua_Render()

	for i=1, #g_Font do
		
		Lfont.Draw(g_Font[i])

	end


	return 0
end

