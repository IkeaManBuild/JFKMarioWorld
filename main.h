#pragma once

void player_code()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError(); return;
	}


	screen(resolution_x, resolution_y, "JFK mario world");
	init_input();
	init_audio();

	Mix_HaltMusic();
	string file = path + "Sounds/music/_boot.ogg";
	music = Mix_LoadMUS(file.c_str());
	Mix_PlayMusic(music, 1);

	string level = "";

	while (true)
	{
		disconnected = false;
		PlayerAmount = 0; SelfPlayerNumber = 1; CheckForPlayers();
		quit = false;

		/* Options Loop */
		string s_or_c;
		cout << yellow << "[JFKMW] Welcome to JFK mario world!, Press R to reload the last level you played, Press Q if you want to play singleplayer, or W to connect to a server, then go to the console. We don't have a UI yet." << white << endl;

		while (true)
		{
			quit = false;
			if (done(true))
			{
				return;
			}
			cls();
			zsnes_ui.process();
			zsnes_ui.draw_button(4, 2, 50, 12);
			zsnes_ui.finish_processing(ren);

			//we create a rectangle for knowing where it will be drawn, in this case it will cover the whole screen.
			SDL_Rect dest;
			dest.x = sp_offset_x; dest.y = sp_offset_y; dest.w = 256 * scale; dest.h = 224 * scale;

			//we copy it to the renderer, for your program, if you want to have a ingame thing (variable or w/e), you just simply don't do this and render what your game has instead.
			SDL_Rect DestR;

			DestR.x = sp_offset_x;
			DestR.y = sp_offset_y;
			DestR.w = 256 * scale;
			DestR.h = 224 * scale;

			//Copied from renderer.h
			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					RenderBackground(
						(-int(double(CameraX) * (double(ServerRAM.RAM[0x3F06]) / 16.0)) % 512) + x * 512,
						-256 + (int(double(CameraY) * (double(ServerRAM.RAM[0x3F07]) / 16.0)) % 512) + y * -512);
				}
			}
			//Copied from renderer.h
			SDL_RenderCopy(ren, screen_t_l1, nullptr, &DestR);
			SDL_RenderCopy(ren, screen_t_l2, nullptr, &DestR);


			SDL_RenderCopy(ren, zsnes_ui.texture, NULL, &dest);

			redraw();
			check_input();
			if (state[SDL_SCANCODE_Q]) {
				s_or_c = "t";
				level = "";
				break;
			}
			if (state[SDL_SCANCODE_W]) {
				s_or_c = "c";
				break;
			}

			if (state[SDL_SCANCODE_R] || SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
			{
				s_or_c = "t";
				break;
			}

		}
		/* Load Shit */

		networking = s_or_c != "t";
		initialize_map16();

		if (s_or_c == "c")
		{
			isClient = true;

#if not defined(DISABLE_NETWORK)

			cout << blue << "[Network] Please input the ip : " << green; cin >> ip;
			cout << blue << "[Network] Please input the port : " << green; cin >> PORT;
			cout << white;
			if (!ConnectClient()) {
				cout << red << "[Network] Failed to connect. Falling back to normal mode." << white << endl;
				networking = false;
				isClient = false;
				disconnected = false;
				s_or_c = "t";
			}
#else
			cout << "Multiplayer is not supported in this build!" << endl;
			networking = false;
			isClient = false;
			s_or_c = "t";
#endif
		}
		
		if (s_or_c == "t")
		{
			isClient = false;
			if (level == "")
			{
				cout << "Enter a level : "; cin >> level;
			}
			LevelManager.LoadLevel(stoi(level, nullptr, 16));
		}

		if (!isClient)
		{
			game_init();
		}

		//Initialize Singleplayer
		if (s_or_c == "t") {
			PlayerAmount = 1; SelfPlayerNumber = 1; CheckForPlayers();
		}

#if not defined(DISABLE_NETWORK)
		//Initialize Multiplayer Client
		if (networking)
		{
			thread = new sf::Thread(&NetWorkLoop); thread->launch();
		}
#endif

		cout << yellow << "[JFKMW] Waiting for player..." << white << endl;
		while (Mario.size() == 0) {
			Sleep(16);
		}
		

		while (!done(true))
		{
			while (doing_read) {
				Sleep(1);
			}

			doing_write = true;
			chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
			check_input(); game_loop(); SoundLoop();

			render();
			chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();

			redraw();
			total_time_ticks = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
			doing_write = false;

			if (disconnected) {
				quit = true; cout << red << "[Network] Disconnected." << white << endl; break;
			}
		}

		//We quit the game go back to the ZSNES ui

#if not defined(DISABLE_NETWORK)
		if (networking) { socketG.disconnect();  thread->terminate(); }
#endif
		cout << yellow << "[JFKMW] Returning to main screen.." << white << endl;
		quit = true;
	}
}