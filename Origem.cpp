/*
	Embedding Lua in C++ Part #1
	"Stupid 3D printer, I didn't want a nest..." - javidx9
	License (OLC-3)
	~~~~~~~~~~~~~~~
	Copyright 2018-2019 OneLoneCoder.com
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	Relevant Video: https://youtu.be/4l5HdmPoynw
	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com
	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019
*/

#include <iostream>
#include <string>

// include Lua, assumes it is local to this file
extern "C"
{
#include "lua542/include/lua.h"
#include "lua542/include/lauxlib.h"
#include "lua542/include/lualib.h"
}

// Link to lua library
#ifdef _WIN32
#pragma comment(lib, "lua542/liblua54.a")
#endif


// Little error checking utility function
bool CheckLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}
	return true;
}


int lua_HostFunction(lua_State* L) {

	//get the values from lua stack
	float a = (float)lua_tonumber(L, 1);
	float b = (float)lua_tonumber(L, 2);
	std::cout << "[C++] HostFunction(" << a << "," << b << ") called" << std::endl;

	float result = a * b;
	//push back onto the lua stack the result
	lua_pushnumber(L, result);
	//cpp function returns the numbers of arguments that will be passed back to lua
	return 1;
}


/*
int lua_HostFunction(lua_State* L)
{
	float a = (float)lua_tonumber(L, 1);
	float b = (float)lua_tonumber(L, 2);
	std::cout << "[CPP S4] HostFunction(" << a << ", " << b << ") called from Lua" << std::endl;
	float c = a * b;
	lua_pushnumber(L, c);
	return 1;
}*/


//check
int VerifyLua(lua_State* L, int r) {

	if (r != LUA_OK) {
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}
	else {
		return true;
	}
}

// NOTE !!!
// YOU WILL NEED THE "EmbeddingLua1.lua" FILE FROM GITHUB REPO


int main()
{

	struct Time {
		int year = 1997;
		int day = 12;
		int month = 02;
		int hour = 14;
		int minute = 43;
		int second = 58;
	};

	Time t1;

	std::cout << t1.day << std::endl;

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	int r = luaL_dofile(L,"EmbeddingLua1.lua");

	if (VerifyLua(L, r)) {

		std::cout << "1" << std::endl;

		lua_getglobal(L,"EditTime");
		if (lua_isfunction(L, -1)) {

			std::cout << "2" << std::endl;
			lua_pushnumber(L, t1.year);
			lua_pushnumber(L, t1.month);
			lua_pushnumber(L, t1.day);
			lua_pushnumber(L, t1.hour);
			lua_pushnumber(L, t1.minute);
			lua_pushnumber(L, t1.second);

			if (VerifyLua(L, lua_pcall(L, 6, 0, 0))) {
		   
			std::cout << "[C++] Inserted time data" << std::endl;	

			}
		}


	}
	

//-----------------------------DAY 12/02/2022-----------------------------------------------------
	/*
	struct Player {
		std::string playerTitle = "none";
		std::string playerName = "none";
		std::string playerFamily = "none";
		int playerLevel = 0;
	};//Will be used to get the Player defined in LuaScript.

	

	Player p1;

	std::cout << t1.day << std::endl;

	std::cout << p1.playerTitle << std::endl;

	lua_State* L = luaL_newstate();//Lua virtual machine
	luaL_openlibs(L);//use a bunch of libraries, to include math functions.
	//register the luafunction HostFunction
	lua_register(L, "HostFunction", lua_HostFunction);
	//Execute the string
	//int r = luaL_dostring(L, cmd.c_str());
	int r = luaL_dofile(L, "EmbeddingLua1.lua");

	//check
	if (VerifyLua(L, r)) {
		//get access to variable a.
		lua_getglobal(L, "DoAThing");
		if (lua_isfunction(L, -1)) {
			//push to number
			lua_pushnumber(L, 3.5f);
			lua_pushnumber(L, 4.2f);
			//lua_pcall - L, how many arguments were provided (a and b), how many arguments will return, how to handle the erros.
			if (VerifyLua(L, lua_pcall(L, 2, 1, 0))) {
				//display the returned value
				std::cout << "[C++] Returned Value processed by Lua Script: " << (float)lua_tonumber(L, -1) << std::endl;
			}
		}

		//First let's call the function
		int playerId = 2;

		lua_getglobal(L, "GetPlayer");
		playerId = 1;
		if (lua_isfunction(L, -1)) {
			lua_pushnumber(L, playerId);//Takes the player according to id.
		}
		if (VerifyLua(L, lua_pcall(L, 1, 1, 0))) {
			//std::cout << "[C++] Returned Value processed by Lua Script: " << (float)lua_tonumber(L, -1) << std::endl;

		//check if the variable is a number, a string, and so on...

			if (lua_istable(L, -1)) {

				lua_pushstring(L, "Title");//Push a new box in the stack
				lua_gettable(L, -2);//gets the box value, not that the table/object advanced a position
				p1.playerTitle = (std::string)lua_tostring(L, -1);//converts the value
				lua_pop(L, 1);//Organize the stack
				std::cout << "playeTitle: " << p1.playerTitle << std::endl;
			}


			if (lua_istable(L, -1)) {

				lua_pushstring(L, "Name");//Push a new box in the stack
				lua_gettable(L, -2);//gets the box value, not that the table/object advanced a position
				p1.playerName = (std::string)lua_tostring(L, -1);//converts the value
				lua_pop(L, 1);//Organize the stack
				std::cout << "playerName: " << p1.playerName << std::endl;
			}

			if (lua_istable(L, -1)) {

				lua_pushstring(L, "Family");//Push a new box in the stack
				lua_gettable(L, -2);//gets the box value, not that the table/object advanced a position
				p1.playerFamily = (std::string)lua_tostring(L, -1);//converts the value
				lua_pop(L, 1);//Organize the stack
				std::cout << "playerFamily: " << p1.playerFamily << std::endl;
			}
			if (lua_istable(L, -1)) {

				lua_pushstring(L, "Level");//Push a new box in the stack
				lua_gettable(L, -2);//gets the box value, not that the table/object advanced a position
				p1.playerLevel = (int)lua_tonumber(L, -1);//converts the value
				lua_pop(L, 1);//Organize the stack
				std::cout << "playerLevel: " << p1.playerLevel << std::endl;
			}


		}

		system("pause");
		//clean the Lua virtual machine, using the close command
		lua_close(L);
		return 0;
//-----------------------------DAY 12/02/2022-----------------------------------------------------
		/*
		struct Player
		{
			std::string title;
			std::string name;
			std::string family;
			int level;
		} player;

		// Create Lua State
		lua_State* L = luaL_newstate();

		// Add standard libraries to Lua Virtual Machine
		luaL_openlibs(L);

		// Register our C++ Function in the global Lua space
		lua_register(L, "HostFunction", lua_HostFunction);


		// Load and parse the Lua File
		if (CheckLua(L, luaL_dofile(L, "EmbeddingLua1.lua")))
		{
			// Stage 1: Just read simple variables
			std::cout << "[CPP] Stage 1 - Read Simple Variables" << std::endl;
			lua_getglobal(L, "a");
			if (lua_isnumber(L, -1)) std::cout << "[CPP S1] a = " << (int)lua_tointeger(L, -1) << std::endl;
			lua_getglobal(L, "b");
			if (lua_isnumber(L, -1)) std::cout << "[CPP S1] b = " << (int)lua_tointeger(L, -1) << std::endl;
			lua_getglobal(L, "c");
			if (lua_isnumber(L, -1)) std::cout << "[CPP S1] c = " << (int)lua_tointeger(L, -1) << std::endl;
			lua_getglobal(L, "d");
			if (lua_isstring(L, -1)) std::cout << "[CPP S1] d = " << lua_tostring(L, -1) << std::endl << std::endl;

			// Stage 2: Read Table Object
			std::cout << "[CPP] Stage 2 - Read Table (Key/Value pairs)" << std::endl;
			lua_getglobal(L, "player");
			if (lua_istable(L, -1))
			{
				lua_pushstring(L, "Name");
				lua_gettable(L, -2);
				player.name = lua_tostring(L, -1);
				lua_pop(L, 1);

				lua_pushstring(L, "Family");
				lua_gettable(L, -2);
				player.family = lua_tostring(L, -1);
				lua_pop(L, 1);

				lua_pushstring(L, "Title");
				lua_gettable(L, -2);
				player.title = lua_tostring(L, -1);
				lua_pop(L, 1);

				lua_pushstring(L, "Level");
				lua_gettable(L, -2);
				player.level = (int)lua_tointeger(L, -1);
				lua_pop(L, 1);
			}
			std::cout << "[CPP S2] " << player.title << " " << player.name << " of " << player.family << " [Lvl: " << player.level << "]" << std::endl << std::endl;

			// Stage 3: Call Lua Function
			std::cout << "[CPP] Stage 3 - Call Lua Function" << std::endl;
			lua_getglobal(L, "CalledFromCPP1");
			if (lua_isfunction(L, -1))
			{
				lua_pushnumber(L, 5.0f);
				lua_pushnumber(L, 6.0f);
				lua_pushstring(L, "Bwa ha ha!");
				std::cout << "[CPP S3] Calling 'CalledFromCPP1' in lua script" << std::endl;
				if (CheckLua(L, lua_pcall(L, 3, 1, 0)))
				{
					std::cout << "[CPP S3] 'CalledFromCPP1' returned " << (float)lua_tonumber(L, -1) << std::endl << std::endl;
				}
			}

			// Stage 4: Call Lua Function, which calls C++ Function
			std::cout << "[CPP] Stage 4 - Call Lua Function, whcih in turn calls C++ Function" << std::endl;
			lua_getglobal(L, "CalledFromCPP2");
			if (lua_isfunction(L, -1))
			{
				lua_pushnumber(L, 5.0f);
				lua_pushnumber(L, 6.0f);
				std::cout << "[CPP S4] Calling 'CalledFromCPP2' in lua script" << std::endl;
				if (CheckLua(L, lua_pcall(L, 2, 1, 0)))
				{
					std::cout << "[CPP S4] 'CalledFromCPP2' returned " << (float)lua_tonumber(L, -1) << std::endl << std::endl;
				}
			}
		}





		system("pause");
		lua_close(L);
		return 0;

		
	}*/
}