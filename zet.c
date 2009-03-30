#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "index.h"

void *luaL_checkuserdata(lua_State *L, int n)
{
  if(lua_isuserdata(L, n)) {
    return lua_touserdata(L, n);
  } else {
    luaL_error(L, "parameter %d should be an index");
    return NULL;
  }
}

static int fn_zet_index_new(lua_State *L)
{
  const char *name = luaL_checkstring(L, 1);
  int memory = luaL_checkint(L, 2);
  struct index *idx;
  idx = index_new(name, NULL, memory, INDEX_NEW_NOOPT, NULL);
  if(idx == 0) {
    luaL_error(L, "Error in index_new('%s',%d): could not create index", name, memory);
    return 0;
  } else {
    lua_pushlightuserdata(L, idx);
    return 1;
  }
}

static int fn_zet_index_load(lua_State *L)
{
  const char *name = luaL_checkstring(L, 1);
  int memory = luaL_checkint(L, 2);
  struct index *idx;
  idx = index_load(name, memory, INDEX_LOAD_NOOPT, NULL);
  if(idx == 0) {
    luaL_error(L, "Error in index_new('%s',%d): could not load index", name, memory);
    return 0;
  } else {
    lua_pushlightuserdata(L, idx);
    return 1;
  }
}

static int fn_zet_index_search(lua_State *L) 
{
  struct index *idx = luaL_checkuserdata(L, 1);
  const char *query = luaL_checkstring(L, 2);
  int startdoc = luaL_checkint(L, 3);
  int maxresults = luaL_checkint(L, 4);
  int nresults;
  double total_results;
  int est;
  struct index_result *results;
  int ret;
  int i;

  struct index_search_opt opt;

  if((maxresults > 200) || (maxresults < 1)) {
    luaL_error(L, "Number of the results requested %d is bad. need between 1 and 200", maxresults);
  }
  results = calloc(sizeof(struct index_result), maxresults);
  if (results == NULL) {
    luaL_error(L, "Could not allocate memory for results");
  }

  opt.summary_type = INDEX_SUMMARISE_CAPITALISE;

  ret = index_search(idx, query, startdoc, maxresults, results, &nresults, &total_results, &est, INDEX_SEARCH_SUMMARY_TYPE, &opt);

  if(ret) {
    lua_createtable(L, nresults, 2);

    lua_pushstring(L, "total");
    lua_pushinteger(L, total_results); 
    lua_rawset(L, -3);

    lua_pushstring(L, "total_is_estimated");
    lua_pushboolean(L, est);
    lua_rawset(L, -3);
    
    for(i=0;i<nresults;i++) {
      lua_pushinteger(L, i+1);
      lua_createtable(L, 0, 5);

      lua_pushstring(L, "docno");
      lua_pushinteger(L, results[i].docno);
      lua_rawset(L, -3);

      lua_pushstring(L, "score");
      lua_pushnumber(L, results[i].score);
      lua_rawset(L, -3);

      lua_pushstring(L, "summary");
      lua_pushstring(L, results[i].summary);
      lua_rawset(L, -3);

      lua_pushstring(L, "title");
      lua_pushstring(L, results[i].title);
      lua_rawset(L, -3);

      lua_pushstring(L, "auxilliary");
      lua_pushstring(L, results[i].auxilliary);
      lua_rawset(L, -3);

      // put this table into parent table's array entries
      lua_rawset(L, -3);
    }
    // The table with the results will be on the stack
    return 1; 
  } else {
    return 0;
  }
}

static const luaL_reg zetlib[] = {
  {"index_new", fn_zet_index_new },
  {"index_load", fn_zet_index_load },
  {"index_search", fn_zet_index_search },
  {NULL, NULL}
};

LUA_API int luaopen_zet (lua_State *L){
  luaL_openlib(L, "zet", zetlib, 0);
  return 1;
}

