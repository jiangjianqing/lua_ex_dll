#include <stdio.h>
#include "main.h"
#include <math.h>
#include <string>
#include "zlib.h"

#if defined(__linux__)
// Linux系统
#include "boost/filesystem.hpp"
using namespace boost::filesystem;
#elif defined(_WIN32)

// Windows系统
#include <filesystem>
using namespace std::tr2::sys;
#endif

using namespace std;

//命令行最大参数个数
#define MAX_ARG_COUNT 100
#ifndef MAX_PATH
#define MAX_PATH 255
#endif

typedef struct compressed_data_s{
    unsigned str_size;
    ulong buf_size; //重点：2018.01.20 教训 ： buf_size一定要申明为ulong 是8byte长度，如果你申明为unsingend,默认为4byte，所以zlib一直返回-5错误（BUF_ERROR）
    char data[];// 柔性数组
}compressed_data_t;

unsigned getCompressBound(unsigned len)
{
    return (unsigned)compressBound((unsigned long)len);
}

bool compressString(const char* str,compressed_data_t* pdata)
{
    int res = compress((Bytef*)pdata->data, &pdata->buf_size, (const Bytef*)str, (unsigned long)pdata->str_size);
    if( res != Z_OK)
    {
        printf("compress failed, error code:%d\n", res);
        return false;
    }
    printf("string size %d, buffer size: %d\n", pdata->str_size, pdata->buf_size);

    return true;
}

bool decompressString(compressed_data_t* pdata, char* str)
{
    printf("string size %d, buffer size: %d\n", pdata->str_size, pdata->buf_size);
    int res = uncompress((Bytef*)str, (unsigned long *)&(pdata->str_size), (const Bytef *)pdata->data, (unsigned long)pdata->buf_size);
    if(res != Z_OK)
    {
        printf("uncompress failed, error code:%d\n", res);
        return false;
    }
    printf("uncompress string:%s\n", str);

    return true;
}

static int decompress_string(lua_State* L)
{
    const char *strBuf = lua_tostring(L, 1);
    compressed_data_t* pdata = (compressed_data_t*)strBuf;
    unsigned slen = pdata->str_size;
    unsigned blen = pdata->buf_size;

    char* str = (char*)malloc(pdata->str_size * sizeof(char));
    if(decompressString(pdata, str)){
        lua_pushlstring(L, str, slen);
    }else{
        lua_pushstring(L, "");
    }

    free(str);
    return 1;
}

static int compress_string(lua_State *L)
{
    size_t slen = 0;
    const char *str = lua_tolstring(L, 1, &slen);
    slen += 1;/* 重点：：：需要把字符串的结束符'\0'也一并处理 */

    uLong blen = compressBound(slen); /* 压缩后的长度是不会超过blen的 */
    compressed_data_t* pdata = (compressed_data_t*)malloc(sizeof(compressed_data_s)+blen*sizeof(char));
    pdata->str_size = slen;
    pdata->buf_size = blen;

    if(compressString(str,pdata)){
        lua_pushlstring(L, (char*)pdata, pdata->buf_size + sizeof(compressed_data_t));
        //printf("sizeof=%d", sizeof(compressed_data_t));

        char* str = (char*)malloc(pdata->str_size * sizeof(char));
        if(decompressString(pdata, str)){

        }else{
            printf("decompressString occur error\n");
        }

    }else{
        lua_pushstring(L, "");
    }
    free(pdata);
    return 1;
}

static const struct luaL_Reg myLib[] =
{
    {"compress_string", compress_string},
    {"decompress_string",decompress_string},
    {NULL, NULL}       //数组中最后一对必须是{NULL, NULL}，用来表示结束
};

/*
** Open my_math library
*/
int luaopen_xzip(lua_State *L)   {
//#ifdef lua5.1
//5.1 下直接使用luaL_register 就好
//    luaL_register(L, "libname", funcs);
//#else //lua5.2
    lua_newtable(L);
    //先把一个table压入VS，然后在调用luaL_setfuncs就会把所以的func存到table中
    //注意不像luaL_register这个table是个无名table，可以在的使用只用一个变量来存入这个table。
    //e.g local clib = require "libname". 这样就不会污染全局环境。比luaL_register更好。
    luaL_setfuncs(L, myLib, 0);
//#endif
  return 1;
}
