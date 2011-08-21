local tm = PHTimer:new(nil,2);
tm:setCallback(function () PHWorld:win() end);
tm:schedule();