#!/usr/bin/lua
local os = require("os")
local socket = require("socket")
local unistd = require("posix.unistd")
local signal = require("posix.signal")
local parser = require("argparse")()

parser:argument "port"
parser:argument "type"
parser:flag "-r" "--recv"

local args = parser:parse(arg)
local _host, _port, _type, _recv = "127.0.0.1", args["port"], args["type"], args["recv"]
local running = true

local _aprs_msg =
    "FLRAAAAAA>APRS,qAS,XXXX:/TIMEh4900.00S\\00800.00E^276/014/A=000000 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4\r\n"
local _sbs_msg = "MSG,3,0,0,BBBBBB,0,TIME,TIME,,1000,,,49.000000,8.000000,,,,,,0\r\n"
local _sens_msg = "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n$WIMWV,242.8,R,6.9,N,A*20\r\n"
local _gps_msg = "$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*49\r\n"

signal.signal(
    signal.SIGINT,
    function(sig)
        running = false
        return 0
    end
)

function Recv(client)
    local m, e = client:receive()
    if not e then
        --print('received from ' .. client:getpeername() .. ': ' .. m)
        print(m)
    elseif e ~= "timeout" then
        error("disconnected")
    end
end

function Send(client, msg)
    local _, e = client:send(msg)
    if not e then
        --print('sent successfully to ' .. client:getpeername())
    elseif e ~= "timeout" then
        error("disconnected")
    end
end

function GenerateMsg()
    if _type == "aprs" then
        return _aprs_msg:gsub("%TIME", os.date("%H%M%S"))
    elseif _type == "sbs" then
        return _sbs_msg:gsub("%TIME", os.date("%Y/%m/%d,%H:%M:%S.000"))
    elseif _type == "sens" then
        return _sens_msg
    elseif _type == "gps" then
        return _gps_msg
    else
        error("no msg")
    end
end

function MakeSender(client)
    return coroutine.create(
        function()
            while running do
                local m, e = GenerateMsg()
                if not e then
                    Send(client, m)
                end
                coroutine.yield()
            end
        end
    )
end

function MakeReceiver(client)
    return coroutine.create(
        function()
            while running do
                Recv(client)
                coroutine.yield()
            end
        end
    )
end

function IsRunning(co)
    return not co or coroutine.status(co) ~= "dead"
end

local server = assert(socket.bind(_host, _port))

--print('Running on port ' .. _port)

while running do
    server:settimeout(1)
    local client = server:accept()
    if client then
        print("Connection from " .. client:getpeername())
        client:settimeout(0)

        local sender, receiver
        sender = MakeSender(client)
        if _recv then
            receiver = MakeReceiver(client)
        end

        while running and IsRunning(sender) and IsRunning(receiver) do
            coroutine.resume(sender)
            if receiver then
                coroutine.resume(receiver)
            end
            unistd.sleep(1)
        end

        print("Disconnected")
        client:close()
    end
end
server:close()
