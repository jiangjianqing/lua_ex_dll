    local utf8 = require('utf8_simple')  
    local str = "你好！"  

        --[[  
            UTF8的编码规则：  
            1. 字符的第一个字节范围： 0x00—0x7F(0-127),或者 0xC2—0xF4(194-244); UTF8 是兼容 ascii 的，所以 0~127 就和 ascii 完全一致  
            2. 0xC0, 0xC1,0xF5—0xFF(192, 193 和 245-255)不会出现在UTF8编码中   
            3. 0x80—0xBF(128-191)只会出现在第二个及随后的编码中(针对多字节编码，如汉字)   
            ]] 
--[[
    function encode_string(s)
	  local encoded = torch.LongTensor(#s)
	  local i = 1
	  -- cause 中文在UTF8中存储占位3个字节，而英文仍然是1个字节[ascii]
	  for token in s.gmatch(s, "[%z\1-\127\194-\244][\128-\191]*") do
	    local idx = self.token_to_idx[token]
	    assert(idx ~= nil, 'Got invalid idx')
	    encoded[i] = idx
	    i = i+1
	  end
	  return encoded
	end
	]]

	--[[
	hello.lua需要保存为gb2312编码，使用notepad编辑保存即可
	如果hello.lua是UTF-8编码，则需要DOS下修改代码页：CHCP 65001 即可。
	]]

--[[
1. utf8字符是变长字符

2. 字符长度有规律

如文字符编码中所列，utf-8是对unicode字符集的编码方案。因此其变长编码方式为：

一字节：0*******

两字节：110*****，10******

三字节：1110****，10******，10******

四字节：11110***，10******，10******，10******

五字节：111110**，10******，10******，10******，10******

六字节：1111110*，10******，10******，10******，10******，10******

因此，拿到字节串后，想判断UTF8字符的byte长度，按照上文的规律，只需要获取该字符的首个Byte，根据其值就可以判断出该字符由几个Byte表示。
]]
local function charsize(ch)
    if not ch then return 0
    elseif ch >=252 then return 6
    elseif ch >= 248 and ch < 252 then return 5
    elseif ch >= 240 and ch < 248 then return 4
    elseif ch >= 224 and ch < 240 then return 3
    elseif ch >= 192 and ch < 224 then return 2
    elseif ch < 192 then return 1
    end
end

-- 截取utf8 字符串
-- str:            要截取的字符串
-- startChar:    开始字符下标,从1开始
-- numChars:    要截取的字符长度
function utf8sub(str, startChar, numChars)
    local startIndex = 1
    while startChar > 1 do
        local char = string.byte(str, startIndex)
        startIndex = startIndex + charsize(char)
        startChar = startChar - 1
    end

    local currentIndex = startIndex

    while numChars > 0 and currentIndex <= #str do
        local char = string.byte(str, currentIndex)
        currentIndex = currentIndex + charsize(char)
        numChars = numChars -1
    end
    return str:sub(startIndex, currentIndex - 1)
end

-- 计算utf8字符串字符数, 各种字符都按一个字符计算
-- 例如utf8len("1你好") => 3
function utf8len(str)
    local len = 0
    local aNum = 0 --字母个数
    local hNum = 0 --汉字个数
    local currentIndex = 1
    while currentIndex <= #str do
        local char = string.byte(str, currentIndex)
        local cs = charsize(char)
        currentIndex = currentIndex + cs
        len = len +1
        if cs == 1 then 
            aNum = aNum + 1
        elseif cs >= 2 then 
            hNum = hNum + 1
        end
    end
    return len, aNum, hNum
end


-- 自测
function test()
    -- test utf8len
    assert(utf8len("你好1世界哈哈") == 7)
    assert(utf8len("你好世界1哈哈 ") == 8)
    assert(utf8len(" 你好世 界1哈哈") == 9)
    assert(utf8len("12345678") == 8)
    assert(utf8len("øpø你好pix") == 8)

    -- test utf8sub
    assert(utf8sub("你好1世界哈哈",2,5) == "好1世界哈")
    assert(utf8sub("1你好1世界哈哈",2,5) == "你好1世界")
    assert(utf8sub(" 你好1世界 哈哈",2,6) == "你好1世界 ")
    assert(utf8sub("你好世界1哈哈",1,5) == "你好世界1")
    assert(utf8sub("12345678",3,5) == "34567")
    assert(utf8sub("øpø你好pix",2,5) == "pø你好p")

    print("all test succ")
end

test()

    -- 获取字符串长度  
    print(str .. " len = " .. utf8.len(str))  
    -- 截取字符串（Lua中字符串索引从1开始）  
    print("sub str = " .. utf8.sub(str,1,2))  