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
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine-master/olcPixelGameEngine.h"

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


class GameWorld : public olc::PixelGameEngine
{
public:
	GameWorld()
	{
		sAppName = "Lua Automation";
	}
	lua_State* script;
	
	olc::vi2d vTileSize = { 32,32 };//load graphics
	olc::Renderable gfx;

	//the level will be defined as an array of tiles
	enum class TileType
	{
		Empty = 0,
		Solid
	};
	//vector to store the tiles themselves
	olc::vi2d vLevelSize;
	std::vector<TileType> vLevel;

	//Dynamic objects
	struct Dynamic
	{
		olc::vf2d pos;
		olc::vf2d size = { 1,1 };
		olc::vf2d vel;

		int id = 0;//if of the object
		bool bDead = false;//flags to tells if the object should be removed from the game or not
	};

	//these Dynamic objecst will be stored in the vector below:
	std::vector<std::shared_ptr<Dynamic>> vDynamicObjects;
	//since the player can only control one object at a Time, it will be created another shared_pointer
	std::shared_ptr<Dynamic> pUnderPlayerControl = nullptr;


public:
	bool OnUserCreate() override
	{
		gfx.Load("./gfx/actors.png");//load graphic in user create
	
		script = luaL_newstate();//define the lua virtual machine
		luaL_openlibs(script);//give all of the lua libraries to lua virtual machine

		lua_register(script,"_CreateLevel", wrap_CreateLevel);//Register the name of the function and binded it to our C++ class.
		lua_register(script, "_SetTile", wrap_SetTile);
		lua_register(script, "_CreateDynamicObject", wrap_createDynamicObject);
		lua_register(script, "_AssignPlayerControl", wrap_AssignPlayerControl);


		if (CheckLua(script, luaL_dofile(script, "Automation1.lua"))) 
		{
			//gets the LocalLevel function from lua script
			lua_getglobal(script, "LoadlLevel");
			//placeback into lua stack and index which will represents the function LocalLevel
			if (lua_isfunction(script,-1)) 
			{
				lua_pushlightuserdata(script, this);//pass a pointer "this" which is the "host" parameter, in other words we passing GameWord object as a host parameter
				lua_pushnumber(script, 1);//inserts the level with value 1
				//then we call the LoadLevel function using lua_pcall
				if(CheckLua(script,lua_pcall(script,2,1,0)))
				{
					return true;
				}


			}
		}
		return false;
	}
	//this function will actually print or draw the tiles to the screen
	bool OnUserUpdate(float fElapsedTime) override
	{

		//Player Controlled Dynamic Object
		if (pUnderPlayerControl != nullptr)
		{
			pUnderPlayerControl->vel = { 0,0 };
			//moves the object around the screen using the keys by setting the velocity and 
			if (GetKey(olc::W).bHeld) pUnderPlayerControl->vel += { 0, -50 };
			if (GetKey(olc::S).bHeld) pUnderPlayerControl->vel += { 0, +50 };
			if (GetKey(olc::A).bHeld) pUnderPlayerControl->vel += { -50, 0 };
			if (GetKey(olc::D).bHeld) pUnderPlayerControl->vel += { +50, 0 };
			//and updating its position accordingly
			pUnderPlayerControl->pos += pUnderPlayerControl->vel * fElapsedTime;
		}




		//Draw level
		Clear(olc::CYAN);//clean the whole screen to CYAN.
		//Create a little 2d vector to iterate all of the tiles in the vector
		olc::vi2d vTile = { 0,0 };
		//used to nested for such as did in lua script
		for (vTile.x = 0; vTile.x < vLevelSize.x; vTile.x++)
		{
			for (vTile.y = 0; vTile.y < vLevelSize.y; vTile.y++)
			{
				//extract which particular tile we're drawing in this instance
				TileType b = vLevel[vTile.y * vLevelSize.x + vTile.x];
				//draw it by checking 
				switch (b)
				{
				case TileType::Empty:
					std::cout << "Empty" << std::endl;
					break;
				case TileType::Solid:
					std::cout << "Solid" << std::endl;
					DrawPartialSprite(vTile * vTileSize, gfx.Sprite(), olc::vi2d(0, 0) * vTileSize, vTileSize);
					break;
				}
			}

		}

		//Draw Dynamic Object
		SetPixelMode(olc::Pixel::ALPHA);
		for (const auto& dynob : vDynamicObjects)
		{
			switch (dynob->id)
			{
			case 0:
				DrawPartialSprite(dynob->pos, gfx.Sprite(), olc::vi2d(1, 0) * vTileSize, vTileSize);
				break;
			case 1:
				DrawPartialSprite(dynob->pos, gfx.Sprite(), olc::vi2d(2, 0) * vTileSize, vTileSize);
				break;
			case 2:
				DrawPartialSprite(dynob->pos, gfx.Sprite(), olc::vi2d(4, 0) * vTileSize, vTileSize);
				break;
			}
		}
		SetPixelMode(olc::Pixel::NORMAL);

		return true;
	}

	//Note: Lua itself cannot see this function, since this is a member of a class
	// Because of this, we'll create a wraper function, which will be the one lua is able to see
	// this function will be wrap_CreateLevel(luaState*L)
	//function _CreateLevel API
	void CreateLevel(int w, int h) 
	{
		vLevelSize = { w,h };//sets our level size vector
		vLevel.clear();//clears out anything in the vector
		vLevel.resize(w * h);//allocate enough space to store all of the elements of the tile map
		
	}

	static int wrap_CreateLevel(lua_State* L)
	{
		//very basic level of error checking, making sure that will be provided three arguments on the stack
		if (lua_gettop(L) != 3) return -1; //host, size.w and size.h
		GameWorld* object = static_cast<GameWorld*>(lua_touserdata(L, 1));
		//The first object provided to lua stack was the pointer "this"lua_pushlightuserdata(script, this);
		//"this" has been transfered to lua as the "host" parameter.
		//When _CreateLevel uses host, we send the pointer back,therefore when we extract that pointer from the lua stack
		//Then we can cast the pointer to an GameWorld object and it will be this gameWorld object.
		//Doubt ? If the pointer was already send before, why we need to cast it again ?
		//Maybe because we're just sending a pointer, not being in paired with the class, or also to avoid errors
		//static cast will allow the compiler to check that the pointer and pointee data types are compatible
		int w = lua_tointeger(L, 2);
		int h = lua_tointeger(L, 3);
		object->CreateLevel(w, h);
		return 0;//returning 0 means we will not be returning any information back to lua.

	}

	void SetTile(int x, int y, int Tile)
	{
		switch (Tile)
		{
		case 0:
			vLevel[y * vLevelSize.x + x] = TileType::Empty;//index the vector then assign the value
			break;
		case 1:
			vLevel[y * vLevelSize.x + x] = TileType::Solid;
			break;

		}
	}

	static int wrap_SetTile(lua_State* L) 
	{
		if (lua_gettop(L) != 4) return -1;
		GameWorld* object = static_cast<GameWorld*>(lua_touserdata(L, 1));
		//takes the other 3 arguments
		int x = lua_tointeger(L, 2);
		int y = lua_tointeger(L, 3);
		int tile_id = lua_tointeger(L, 4);
		object->SetTile(x, y, tile_id);
		return 0;//returning 0 means we will not be returning any information back to lua.

	}

	std::shared_ptr<Dynamic> createDynamicObject(int type, float x, float y) 
	{
		//creates the dynamicObject
		auto dynob = std::make_shared<Dynamic>();
		dynob->id = type;
		dynob->pos = { x, y };
		//push_back the object in the vector to be stored
		vDynamicObjects.push_back(dynob);
		return dynob;
	}

	static int wrap_createDynamicObject(lua_State* L)
	{
		if (lua_gettop(L) != 4) return -1;
		GameWorld* object = static_cast<GameWorld*>(lua_touserdata(L, 1));
		int type = lua_tointeger(L, 2);
		int x = lua_tointeger(L, 3);
		int y = lua_tointeger(L, 4);
		//push a ptr to lua stack
		lua_pushlightuserdata(L, object->createDynamicObject(type, x, y).get());//.get() will access the raw pointer underneath the shared pointer
		return 1;//returning 1 tells lua virtual machine we'll return item of data, in this case the object ptr.
	}

	void AssignPlayerControl(Dynamic* dynob)
	{
		//When the smart pointer lua_pushlightuserdata(L, object->createDynamicObject(type, x, y).get());, went out to lua, it is passed as a raw pointer
		//Dynamic is smart pointer, so find corresponding smart pointer
		//Note this can be improved
		auto ptr = std::find_if(vDynamicObjects.begin(), vDynamicObjects.end(), [&dynob](const std::shared_ptr<Dynamic>& p) {return p.get() == dynob; });
		//I guess, what is happening it it:
		//First the object was passed to lua, shared_ptr was passed a raw ptr.
		//Then we want to use the same object by giving the ownsership in this scope, so we take the object dynob, find the previous object created before passing to lua, and assign it to auto.
		//then, we return the shared_ptr state again.
		pUnderPlayerControl = *ptr;
	}

	static int wrap_AssignPlayerControl(lua_State* L)
	{
		if (lua_gettop(L) != 2) return -1;
		GameWorld* object = static_cast<GameWorld*>(lua_touserdata(L, 1));
		Dynamic* dynob = static_cast<Dynamic*>(lua_touserdata(L, 2));
		object->AssignPlayerControl(dynob);
		return 0;
	}




};

int main() {
//	std::cout << "Teste 1" << std::endl;
	GameWorld	demo;
	if (demo.Construct(512, 480, 2, 2))//Game pixel parameters, 2,2 will impact on our resolution. make a window with resolution 4320x1840
		demo.Start();
	return 0;
}

//**Previous class and editions**************************************************************************************

// Little error checking utility function
//bool CheckLua(lua_State* L, int r)
//{
//	if (r != LUA_OK)
//	{
//		std::string errormsg = lua_tostring(L, -1);
//		std::cout << errormsg << std::endl;
//		return false;
//	}
//	return true;
//}
//
//
//int lua_HostFunction(lua_State* L) {
//
//	//get the values from lua stack
//	float a = (float)lua_tonumber(L, 1);
//	float b = (float)lua_tonumber(L, 2);
//	std::cout << "[C++] HostFunction(" << a << "," << b << ") called" << std::endl;
//
//	float result = a * b;
//	//push back onto the lua stack the result
//	lua_pushnumber(L, result);
//	//cpp function returns the numbers of arguments that will be passed back to lua
//	return 1;
//}
//
//
///*
//int lua_HostFunction(lua_State* L)
//{
//	float a = (float)lua_tonumber(L, 1);
//	float b = (float)lua_tonumber(L, 2);
//	std::cout << "[CPP S4] HostFunction(" << a << ", " << b << ") called from Lua" << std::endl;
//	float c = a * b;
//	lua_pushnumber(L, c);
//	return 1;
//}*/
//
//
////check
//int VerifyLua(lua_State* L, int r) {
//
//	if (r != LUA_OK) {
//		std::string errormsg = lua_tostring(L, -1);
//		std::cout << errormsg << std::endl;
//		return false;
//	}
//	else {
//		return true;
//	}
//}
//
//// NOTE !!!
//// YOU WILL NEED THE "EmbeddingLua1.lua" FILE FROM GITHUB REPO
//
//
//int main()
//{
//
//	struct Time {
//		int year = 1997;
//		int day = 12;
//		int month = 02;
//		int hour = 14;
//		int minute = 43;
//		int second = 58;
//	};
//
//	Time t1;
//
//	std::cout << t1.day << std::endl;
//	std::cout << "testeMR" << std::endl;
//	std::cout << "testeMR2" << std::endl;
//
//
//	lua_State* L = luaL_newstate();
//	luaL_openlibs(L);
//	int r = luaL_dofile(L,"EmbeddingLua1.lua");
//
//	if (VerifyLua(L, r)) {
//
//		std::cout << "1" << std::endl;
//
//		lua_getglobal(L,"EditTime");
//		if (lua_isfunction(L, -1)) {
//
//			std::cout << "2" << std::endl;
//			lua_pushnumber(L, t1.year);
//			lua_pushnumber(L, t1.month);
//			lua_pushnumber(L, t1.day);
//			lua_pushnumber(L, t1.hour);
//			lua_pushnumber(L, t1.minute);
//			lua_pushnumber(L, t1.second);
//
//			if (VerifyLua(L, lua_pcall(L, 6, 0, 0))) {
//		   
//			std::cout << "[C++] Inserted time data" << std::endl;	
//
//			}
//		}
//
//
//	}
	

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
//}