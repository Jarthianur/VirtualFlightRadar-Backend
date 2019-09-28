#!/usr/bin/lua
local socket = require('socket')
local unistd = require('posix.unistd')
local signal = require('posix.signal')
local parser = require('argparse')()

parser:argument 'port'
parser:option '-m' '--msg'
parser:flag '-r' '--recv'

local args = parser:parse(arg)
local _host, _port, _msg, _recv = '127.0.0.1', args['port'], args['msg'], args['recv']
local running = true

if _msg then
    _msg = _msg .. '\r\n'
end

signal.signal(
    signal.SIGINT,
    function(sig)
        running = false
        return 0
    end
)

function recv(client)
    local m, e = client:receive()
    if not e then
        --print('received from ' .. client:getpeername() .. ': ' .. m)
        print(m)
    elseif e ~= 'timeout' then
        error('disconnected')
    end
end

function send(client, msg)
    local _, e = client:send(msg)
    if not e then
        --print('sent successfully to ' .. client:getpeername())
    elseif e ~= 'timeout' then
        error('disconnected')
    end
end

function makesender(client)
    return coroutine.create(
        function()
            while running do
                send(client, _msg)
                coroutine.yield()
            end
        end
    )
end

function makereceiver(client)
    return coroutine.create(
        function()
            while running do
                recv(client)
                coroutine.yield()
            end
        end
    )
end

function isrunning(co)
    return not co or coroutine.status(co) ~= 'dead'
end

local server = assert(socket.bind(_host, _port))

--print('Running on port ' .. _port)

while running do
    server:settimeout(1)
    local client = server:accept()
    if client then
        print('Connection from ' .. client:getpeername())
        client:settimeout(0)

        local sender, receiver
        if _msg then
            sender = makesender(client)
        end
        if _recv then
            receiver = makereceiver(client)
        end

        while running and isrunning(sender) and isrunning(receiver) do
            if sender then
                coroutine.resume(sender)
            end
            if receiver then
                coroutine.resume(receiver)
            end
            unistd.sleep(1)
        end

        print('Disconnected')
        client:close()
    end
end
server:close()
