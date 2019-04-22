-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.


local Sedan = Class()

local UHeadMountedDisplayFunctionLibrary = UE4.FindClass('HeadMountedDisplayFunctionLibrary')
local EOrientPositionSelector = SUE4Lua.GetEnumTable('EOrientPositionSelector')

local function EnableIncarView(self, State)
    self.IsInCar = State
    
    if self.IsInCar then
        -- Enable HMD follow HMD Orientation
        self:OnResetVR({})

        -- Activate Internal, Deactivate chase
        self.InternalCamera:Activate()
        self.ChaseCamera:Deactivate()
    else
        -- Deactivate Internal, Activate chase
        self.InternalCamera:Deactivate()
        self.ChaseCamera:Activate()
    end

    -- Update Incar HUD visibility
    self.InCarGear:SetVisibility(self.IsInCar)
    self.InCarSpeed:SetVisibility(self.IsInCar)
end

local function UpdateHUDStrings(self)
    -- Setup String for Speed
    self.SpeedDisplayString = string.format("%dkm/h",
        -- Convert from units/sec to kilometers/hour
        math.floor(math.abs(self.VehicleMovement:GetForwardSpeed() * 0.036))
    )

    -- Setup string for Gear
    if self.InReverseGear then
        self.GearDisplayString = "R"
    else
        local CurrentGear = self.VehicleMovement:GetCurrentGear()
    
        if 0 < CurrentGear then
            self.GearDisplayString = tostring(CurrentGear)
        else
            self.GearDisplayString = "N"
        end
    end
end

local function SetupIncarHUD(self)
    --Setup Gear Display
    self.InCarGear:SetText(self.GearDisplayString)
    self.InCarGear:SetTextRenderColor(UE4.LinearColor.ToColor(self.InReverseGear and self.GearDisplayReverseColor or self.GearDisplayColor))
    
    -- Setup speed display
    self.InCarSpeed:SetText(self.SpeedDisplayString)    
end

function Sedan:ReceiveBeginPlay(Params)
    SUE4Lua.Log("Sedan:ReceiveBeginPlay() was called.")

    -- Set Default Camera
    local bHMDEnabled = UHeadMountedDisplayFunctionLibrary.IsHeadMountedDisplayEnabled()
    EnableIncarView(self, bHMDEnabled)
    if bHMDEnabled then
        SUE4Lua.Log("HMD enabled.")

        -- ResetOrientationAndPosition() has default parameters and we don't have to pass arguments.
        -- This is example code for the GetEnumTable().
        UHeadMountedDisplayFunctionLibrary.ResetOrientationAndPosition(
            0,
            EOrientPositionSelector.OrientationAndPosition
        )
    end 
end

function Sedan:ReceiveTick(Params)    
    -- Update
    self.InReverseGear = self.VehicleMovement:GetCurrentGear() < 0
    UpdateHUDStrings(self)
    SetupIncarHUD(self)

    -- Handle Mouse camera movement of internal camera
    if self.IsInCar and not UHeadMountedDisplayFunctionLibrary.IsHeadMountedDisplayEnabled() then
        local NewRotation = self.InternalCamera.RelativeRotation
        NewRotation.Pitch = UE4.Math.ClampAngle(NewRotation.Pitch + self:GetLookUp(), -89, 89)
        NewRotation.Yaw = NewRotation.Yaw + self:GetLookRight()
        self.InternalCamera:SetRelativeRotation({
            NewRotation = NewRotation,
            bSweep = false,
            bTeleport = false,
        })
    end
end

function Sedan:OnResetVR(Params)
    SUE4Lua.Log("Sedan:OnResetVR() was called.")

    local Key = Params.Key

    UHeadMountedDisplayFunctionLibrary.ResetOrientationAndPosition()
end

function Sedan:OnSwitchCamera(Params)
    SUE4Lua.Log("Sedan:OnSwitchCamera() was called.")

    -- Toggle Camera
    EnableIncarView(self, not self.IsIncar)
end

function Sedan:OnMoveRight(Params)
    --SUE4Lua.Log("Sedan:OnMoveRight() was called.")

    local AxisValue = Params.AxisValue

    -- Steering inupt
    self.VehicleMovement:SetSteeringInput(AxisValue)
end

function Sedan:OnMoveForward(Params)
    --SUE4Lua.Log("Sedan:OnMoveForward() was called.")

    local AxisValue = Params.AxisValue
    
    -- Throttle inupt
    self.VehicleMovement:SetThrottleInput(AxisValue)
end

function Sedan:OnHandbrake(Params)
    SUE4Lua.Log("Sedan:OnHandbrake() was called.")

    local bNewHandbrake = Params.bNewHandbrake
    
    -- Handbrake inupt
    self.VehicleMovement:SetHandbrakeInput(bNewHandbrake)
end

return Sedan