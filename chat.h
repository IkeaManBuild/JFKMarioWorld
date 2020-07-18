#pragma once

string Curr_ChatString = "";
string Old_ChatString = "";
int Time_ChatString = 0; //How long it will take for this string to disappear, set to 300 every time a message is sent. In frames

void Chat_ServerSide()
{
	for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		MPlayer& CurrPlayer = *item;
		if (CurrPlayer.last_chat_string != CurrPlayer.curr_chat_string && CurrPlayer.curr_chat_string != "")
		{
			CurrPlayer.last_chat_string = CurrPlayer.curr_chat_string;
			Curr_ChatString = CurrPlayer.curr_chat_string;
			Time_ChatString = 420;
			cout << lua_color << "[Chat S] " << Curr_ChatString << white << endl;

			lua_on_chatted(Curr_ChatString);

			if (!isClient && networking)
			{
				string New_ChatString = Curr_ChatString;
				replaceAll(New_ChatString, "<", "");
				replaceAll(New_ChatString, ">", ":");
				replaceAll(New_ChatString, ",", "");

				discord_message(New_ChatString);
			}
		}
	}

	if (Time_ChatString > 0)
	{
		Time_ChatString--;
		if (Time_ChatString == 0)
		{
			Curr_ChatString = "";
		}
	}
}

void Chat_ClientSide()
{
	if (Old_ChatString != Curr_ChatString && Curr_ChatString != "")
	{
		Old_ChatString = Curr_ChatString;
		Time_ChatString = 420;
		cout << lua_color << "[Chat C] " << Curr_ChatString << white << endl;

		MPlayer& LocalPlayer = get_mario(SelfPlayerNumber);
		LocalPlayer.Chat(Typing_In_Chat);
		LocalPlayer.curr_chat_string = "";
	}

	if (Time_ChatString > 0)
	{
		Time_ChatString--;
	}

	if (Chatting)
	{
		for (int i = 0x41; i <= 0x5A; i++)
		{
			if (getKey(i))
			{
				Typing_In_Chat += (i-0x10) + '0';
			}
		}
		//Numbers
		for (int i = 0x30; i <= 0x39; i++)
		{
			if (getKey(i))
			{
				Typing_In_Chat += (i - 0x30) + '0';
			}
		}

		if (getKey(0x20))
		{
			Typing_In_Chat += " ";
		}

		//Delete last
		if (getKey(0x08) && Typing_In_Chat.size() > 0) { Typing_In_Chat.pop_back(); }

		Typing_In_Chat = Typing_In_Chat.substr(0, min(55, int(Typing_In_Chat.length())));
	}
	bool stat = Chatting ? (state[input_settings[9]]) : state[input_settings[10]];
	if (stat != pressed_start)
	{
		pressed_start = stat;
		if (stat)
		{
			Chatting = !Chatting;
			if (Chatting == false)
			{

				MPlayer& LocalPlayer = get_mario(SelfPlayerNumber);
				LocalPlayer.Chat(Typing_In_Chat);
			}
			else
			{
#if defined(_WIN32)
				KeyStates[0x54] = true;
#endif
			}
			Typing_In_Chat = "";
		}
	}
}

void ProcessChat()
{
	if (!isClient)
	{
		Chat_ServerSide();
	}

	if (isClient || !networking)
	{
		Chat_ClientSide();
	}
}