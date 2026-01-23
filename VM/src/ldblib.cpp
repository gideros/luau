// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#include "lualib.h"

#include "lvm.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static lua_State* getthread(lua_State* L, int* arg)
{
    if (lua_isthread(L, 1))
    {
        *arg = 1;
        return lua_tothread(L, 1);
    }
    else
    {
        *arg = 0;
        return L;
    }
}

static int db_info(lua_State* L)
{
    int arg;
    lua_State* L1 = getthread(L, &arg);
    int l1top = 0;

    // if L1 != L, L1 can be in any state, and therefore there are no guarantees about its stack space
    if (L != L1)
    {
        // for 'f' option, we reserve one slot and we also record the stack top
        lua_rawcheckstack(L1, 1);

        l1top = lua_gettop(L1);
    }

    int level;
    if (lua_isnumber(L, arg + 1))
    {
        level = (int)lua_tointeger(L, arg + 1);
        luaL_argcheck(L, level >= 0, arg + 1, "level can't be negative");
    }
    else if (arg == 0 && lua_isfunction(L, 1))
    {
        // convert absolute index to relative index
        level = -lua_gettop(L);
    }
    else
        luaL_argerror(L, arg + 1, "function or level expected");

    const char* options = luaL_checkstring(L, arg + 2);

    lua_Debug ar;
    if (!lua_getinfo(L1, level, options, &ar))
        return 0;

    int results = 0;
    bool occurs[26] = {};

    for (const char* it = options; *it; ++it)
    {
        if (unsigned(*it - 'a') < 26)
        {
            if (occurs[*it - 'a'])
            {
                // restore stack state of another thread as 'f' option might not have been visited yet
                if (L != L1)
                    lua_settop(L1, l1top);

                luaL_argerror(L, arg + 2, "duplicate option");
            }
            occurs[*it - 'a'] = true;
        }

        switch (*it)
        {
        case 's':
            lua_pushstring(L, ar.short_src);
            results++;
            break;

        case 'l':
            lua_pushinteger(L, ar.currentline);
            results++;
            break;

        case 'n':
            lua_pushstring(L, ar.name ? ar.name : "");
            results++;
            break;

        case 'f':
            if (L1 == L)
                lua_pushvalue(L, -1 - results); // function is right before results
            else
                lua_xmove(L1, L, 1); // function is at top of L1
            results++;
            break;

        case 'a':
            lua_pushinteger(L, ar.nparams);
            lua_pushboolean(L, ar.isvararg);
            results += 2;
            break;

        default:
            // restore stack state of another thread as 'f' option might not have been visited yet
            if (L != L1)
                lua_settop(L1, l1top);

            luaL_argerror(L, arg + 2, "invalid option");
        }
    }

    return results;
}

static void treatstackoption (lua_State *L, lua_State *L1, const char *fname) {
  if (L == L1) {
    lua_pushvalue(L, -2);
    lua_remove(L, -3);
  }
  else
    lua_xmove(L1, L, 1);
  lua_setfield(L, -2, fname);
}

static int db_getinfo(lua_State* L)
{
    int arg;
    lua_State* L1 = getthread(L, &arg);

    // If L1 != L, L1 can be in any state, and therefore there are no guarantees about its stack space
    if (L != L1)
        lua_rawcheckstack(L1, 1); // for 'f' option

    int level;
    if (lua_isnumber(L, arg + 1))
    {
        level = (int)lua_tointeger(L, arg + 1);
        luaL_argcheck(L, level >= 0, arg + 1, "level can't be negative");
    }
    else if (arg == 0 && lua_isfunction(L, 1))
    {
        // convert absolute index to relative index
        level = -lua_gettop(L);
    }
    else
        luaL_argerror(L, arg + 1, "function or level expected");

    const char* options = luaL_checkstring(L, arg + 2);

    lua_Debug ar;
    if (!lua_getinfo(L1, level, options, &ar))
        return 0;

    bool occurs[26] = {};

    lua_newtable(L);
    for (const char* it = options; *it; ++it)
    {
        if (unsigned(*it - 'a') < 26)
        {
            if (occurs[*it - 'a'])
                luaL_argerror(L, arg + 2, "duplicate option");
            occurs[*it - 'a'] = true;
        }

        switch (*it)
        {
        case 'S':
            lua_pushstring(L, ar.source);
            lua_setfield(L,-2,"source");
            lua_pushstring(L, ar.short_src);
            lua_setfield(L,-2,"short_src");
            lua_pushinteger(L, ar.linedefined);
            lua_setfield(L,-2,"linedefined");
            lua_pushstring(L, ar.what);
            lua_setfield(L,-2,"what");
            break;

        case 's':
            lua_pushstring(L, ar.short_src);
            lua_setfield(L,-2,"short_src");
            break;

        case 'l':
            lua_pushinteger(L, ar.currentline);
            lua_setfield(L,-2,"currentline");
            break;

        case 'n':
            lua_pushstring(L, ar.name ? ar.name : "");
            lua_setfield(L,-2,"name");
            lua_pushstring(L, "");
            lua_setfield(L,-2,"namewhat");
            break;
        case 'f':
            treatstackoption(L, L1, "func");
            break;
        default:
            luaL_argerror(L, arg + 2, "invalid option");
        }
    }

    return 1;
}


static int db_traceback(lua_State* L)
{
    int arg;
    lua_State* L1 = getthread(L, &arg);
    const char* msg = luaL_optstring(L, arg + 1, NULL);
    int level = luaL_optinteger(L, arg + 2, (L == L1) ? 1 : 0);
    luaL_argcheck(L, level >= 0, arg + 2, "level can't be negative");

    luaL_traceback(L, L1, msg, level);
    return 1;
}

static int db_getregistry (lua_State *L) {
  lua_pushvalue(L, LUA_REGISTRYINDEX);
  return 1;
}

//For MobDebug
static int db_getlocal (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  const char *name;
  name=lua_getlocal(L1,luaL_checkint(L, arg+1),luaL_checkint(L, arg+2));
  if (name) {
    lua_xmove(L1, L, 1);
    lua_pushstring(L, name);
    lua_pushvalue(L, -2);
    return 2;
  }
  else {
    lua_pushnil(L);
    return 1;
  }
}


static int db_setlocal (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  luaL_checkany(L, arg+3);
  lua_settop(L, arg+3);
  lua_xmove(L, L1, 1);
  lua_pushstring(L, lua_setlocal(L1,luaL_checkint(L, arg+1),luaL_checkint(L, arg+2)));
  return 1;
}


static int auxupvalue (lua_State *L, int get) {
  const char *name;
  int n = luaL_checkint(L, 2);
  luaL_checktype(L, 1, LUA_TFUNCTION);
  if (lua_iscfunction(L, 1)) return 0;  /* cannot touch C upvalues from Lua */
  name = get ? lua_getupvalue(L, 1, n) : lua_setupvalue(L, 1, n);
  if (name == NULL) return 0;
  lua_pushstring(L, name);
  lua_insert(L, -(get+1));
  return get + 1;
}


static int db_getupvalue (lua_State *L) {
  return auxupvalue(L, 1);
}


static int db_setupvalue (lua_State *L) {
  luaL_checkany(L, 3);
  return auxupvalue(L, 0);
}

static const char KEY_HOOK = 'h';

#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4

static int lastLine=0;
static int stackCount=0;
#include <string>
static std::string lastFile;
static bool inhook=false;

static void hookg (lua_State *L, int event, int line) {
    if (inhook) return;
    inhook=true;

  static const char *const hooknames[] =
    {"call", "return", "line", "count", "tail return"};
  lua_pushlightuserdata(L, (void *)&KEY_HOOK);
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, L);
  lua_rawget(L, -2);
  if (lua_isfunction(L, -1)) {
    lua_pushstring(L, hooknames[event]);
    if (line >= 0)
      lua_pushinteger(L, line);
    else lua_pushnil(L);
    lua_call(L, 2, 0);
  }
  inhook=false;
}

static void hookp(lua_State *L,int enter) {
    if (inhook) return;
     if (enter)
         stackCount++;
     else
         stackCount--;
}

static void hookf (lua_State *L, lua_Debug *ar) {
    if (inhook) return;
    while (stackCount>0) {
        hookg(L,LUA_HOOKCALL,-1);
        stackCount--;
    }
    while (stackCount<0) {
        hookg(L,LUA_HOOKRET,-1);
        stackCount++;
    }

    lua_getinfo(L,0,"s",ar);
    std::string src=ar->source;
    if ((lastFile==src)&&(lastLine==ar->currentline)) return;
    lastFile=src;
    lastLine=ar->currentline;
    if (ar->currentline<0) //Line can't be determined, continue
        return;
    hookg(L,LUA_HOOKLINE,ar->currentline);
}

static void gethooktable (lua_State *L) {
  lua_pushlightuserdata(L, (void *)&KEY_HOOK);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    lua_createtable(L, 0, 1);
    lua_pushlightuserdata(L, (void *)&KEY_HOOK);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }
}

#include "lstate.h"
static int db_sethook (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  if (lua_isnoneornil(L, arg+1)) {
    lua_settop(L, arg+1);
  }
  else {
    luaL_checktype(L, arg+1, LUA_TFUNCTION);
  }
  gethooktable(L);
  lua_pushlightuserdata(L, L1);
  lua_pushvalue(L, arg+1);
  lua_rawset(L, -3);  /* set new hook */
  lua_pushnil(L);
  int more=lua_next(L,-2);
  lua_pop(L,more);
  lua_pop(L, 1);  /* remove hook table */

  L1->global->cb.debugstep=more?hookf:NULL;
  L1->profilerHook=more?hookp:NULL;
  lua_singlestep(L1,more?1:0);

  return 0;
}

static const luaL_Reg dblib[] = {
    {"info", db_info},
    {"getinfo", db_getinfo},
    {"traceback", db_traceback},
	//Gideros extra
	{"getregistry", db_getregistry},
    //For MobDebug
    {"getlocal", db_getlocal},
    {"getupvalue", db_getupvalue},
    {"sethook", db_sethook},
    {"setlocal", db_setlocal},
    {"setupvalue", db_setupvalue},
    {NULL, NULL},
};

int luaopen_debug(lua_State* L)
{
    luaL_register(L, LUA_DBLIBNAME, dblib);
    return 1;
}
