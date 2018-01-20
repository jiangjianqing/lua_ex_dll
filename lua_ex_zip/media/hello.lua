--搜索指定路径下，以 .lua结尾的文件
--package.path = "../h/?.lua;../ybslib/src/?.lua;"..package.path
--搜索指定路径下，以.so 或.dll 结尾的文件
--package.cpath = "../ybslib/bin/?.so;"..package.cpath
--package.cpath = "../ybslib/bin/?.dll;"..package.cpath

local ZipUtils= require "xzip"

local string = "test1236879685 z中文字符可以吗？"  
local zipData = ZipUtils.compress_string(string)  
if zipData ~= "" then  
    print("XXXXXXXXXXXXXXXXX compress", zipData)  
else  
    print("XXXXXXXXXXXXXXXXX error")  
end  
local str = ZipUtils.uncompress_string(zipData)  
if str ~= "" then  
    print("XXXXXXXXXXXXXXXXX decompress", str)  
else  
    print("XXXXXXXXXXXXXXXXX error")  
end 



