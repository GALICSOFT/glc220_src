

function GameBeginFrameMove()

	if  250 < g_mouse_x and g_mouse_x <370 and
		250 < g_mouse_y and g_mouse_y <300 then

		if g_mouse_e == KEY_DOWN then
			g_gamePhase	= GAMEPHASE_PLAY		-- play
			g_handUsr = PLAY_N
			g_handCom = Lutil.Rand(3)
		end
	end


	if  290 < g_mouse_x and g_mouse_x <340 and
		320 < g_mouse_y and g_mouse_y <370 then

		if g_mouse_e == KEY_DOWN then
			g_gamePhase	= GAMEPHASE_EXIT		-- exit
		end
	end

end



function GameBeginRender()

	local UI_START     = 1
	local UI_END       = 2

	local st_tx

	st_tx = g_uibg[1];			Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)
	st_tx = g_uibg[2];			Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)


	st_tx = g_uibtn[UI_START]
	if  250 < g_mouse_x and g_mouse_x <370 and
		250 < g_mouse_y and g_mouse_y <300 then

		Ltex.Draw( st_tx.o, st_tx.x, st_tx.y)
	else
		Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)
	end


	st_tx = g_uibtn[UI_END]
	if  290 < g_mouse_x and g_mouse_x <340 and
		320 < g_mouse_y and g_mouse_y <370 then

		Ltex.Draw( st_tx.o, st_tx.x, st_tx.y)
	else
		Ltex.Draw( st_tx.n, st_tx.x, st_tx.y)
	end


end



