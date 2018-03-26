--[[

Lua Boobie Servo Test Script
Andrew Armstrong
www.backofficeshow.com

]]

local comport = 6
local servomin = 0
local servomax = 20

function sleep(s)
  local ntime = os.clock() + s
  repeat until os.clock() > ntime
end

function moveservo(servo, value)
	local padservo = string.format("%02d", servo) --format correctly for Boobie
	local padvalue = string.format("%02d", value)
	local command = "echo " .. padservo .. padvalue .. " > COM" .. comport
	os.execute(command)
end

--Reset all channels to neutral:
os.execute("echo t > COM" .. comport)

--Lets run the through their extents
for i=6,11 do
	moveservo(i,servomin)
end

for i=6,11 do
	moveservo(i,servomax)
end


-- A little wiggle example for laserbot:

for i=1,10 do
	moveservo(11, 4)
	moveservo(10, 3)
	sleep(1) --let the servos catch up so we can see what is happening!
	moveservo(11, 14)
	moveservo(10, 15)
	sleep(1)
end