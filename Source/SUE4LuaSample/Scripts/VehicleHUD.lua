-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.


local VehicleHUD = Class()

function VehicleHUD:ReceiveDrawHUD(Params)
    --SUE4Lua.Log("VehicleHUD:ReceiveDrawHUD() was called.")

    local SizeX = Params.SizeX
    local SizeY = Params.SizeY

    local OwningPawn = self:GetOwningPawn()
    local Sedan = (UE4.IsValid(OwningPawn) and OwningPawn:IsA('Sedan_C')) and OwningPawn or nil
    
    if Sedan ~= nil and not Sedan.IsInCar then
        -- Calculate ratio from 720p
        local HUDXRatio, HUDYRatio = 0.0, 0.0

        HUDXRatio = SizeX / 1280.0
        HUDYRatio = SizeY / 720.0 

        -- Print vehicle speed
        self:DrawText({
            Text = Sedan.SpeedDisplayString,
            TextColor = UE4.LinearColor.new(1.0, 1.0, 1.0, 1.0),
            ScreenX = HUDXRatio * 805,
            ScreenY = HUDYRatio * 455,
            Font = self.Font,
            Scale = HUDYRatio * 1.4,
        })

        -- Print vehicle gear
        self:DrawText({
            Text = "Gear:" .. Sedan.GearDisplayString,
            TextColor = Sedan.InReverseGear and Sedan.GearDisplayReverseColor or Sedan.GearDisplayColor,
            ScreenX = HUDXRatio * 805,
            ScreenY = HUDYRatio * 500,
            Font = self.Font,
            Scale = HUDYRatio * 1.4,
        })

    end
end

return VehicleHUD