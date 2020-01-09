-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.


UE4 = {}

local PrimitiveType = Class()
PrimitiveType.isPrimitiveType = true

--=============================================================================================================================
-- PlatformTime
--=============================================================================================================================
UE4.PlatformTime = {}

--=============================================================================================================================
-- FVector
--=============================================================================================================================
UE4.Vector = Class(PrimitiveType)
function UE4.Vector:init(x, y, z)
	self.X = x or 0.0
	self.Y = y or 0.0
	self.Z = z or 0.0
end
UE4.Vector.ZeroVector = UE4.Vector.new(0.0, 0.0, 0.0)
UE4.Vector.OneVector = UE4.Vector.new(1.0, 1.0, 1.0)

function UE4.Vector:GetNormal2D(tolerance)
	return UE4.Vector.new(self.X, self.Y, 0.0):GetNormal(tolerance)
end

--=============================================================================================================================
-- FVector2D
--=============================================================================================================================
UE4.Vector2D = Class(PrimitiveType)
function UE4.Vector2D:init(x, y)
	self.X = x or 0.0
	self.Y = y or 0.0
end
UE4.Vector2D.ZeroVector = UE4.Vector2D.new(0.0, 0.0)

--=============================================================================================================================
-- FVector4
--=============================================================================================================================
UE4.Vector4 = Class(PrimitiveType)
function UE4.Vector4:init(x, y, z, w)
	self.X = x or 0.0
	self.Y = y or 0.0
	self.Z = z or 0.0
	self.W = w or 0.0
end
UE4.Vector4.ZeroVector = UE4.Vector4.new(0.0, 0.0, 0.0, 0.0)

--=============================================================================================================================
-- FIntPoint
--=============================================================================================================================
UE4.IntPoint = Class(PrimitiveType)
function UE4.IntPoint:init(x, y)
	self.X = x or 0
	self.Y = y or 0
end
UE4.IntPoint.ZeroValue = UE4.Vector4.new(0, 0)

--=============================================================================================================================
-- FIntVector
--=============================================================================================================================
UE4.IntVector = Class(PrimitiveType)
function UE4.IntVector:init(x, y, z)
	self.X = x or 0
	self.Y = y or 0
	self.Z = z or 0
end
UE4.IntVector.ZeroValue = UE4.Vector4.new(0, 0, 0)

--=============================================================================================================================
-- FPlane
--=============================================================================================================================
UE4.Plane = Class(PrimitiveType)
function UE4.Plane:init(x, y, z, w)
	self.X = x or 0.0
	self.Y = y or 0.0
	self.Z = z or 0.0
	self.W = w or 0.0
end

--=============================================================================================================================
-- FMatrix
--=============================================================================================================================
UE4.Matrix = Class(PrimitiveType)
function UE4.Matrix:init(x, y, z, w)
	self.XPlane = x or UE4.Plane.new()
	self.YPlane = y or UE4.Plane.new()
	self.ZPlane = z or UE4.Plane.new()
	self.WPlane = w or UE4.Plane.new()
end

--=============================================================================================================================
-- FRotator
--=============================================================================================================================
UE4.Rotator = Class(PrimitiveType)
function UE4.Rotator:init(pitch, yaw, roll)
	self.Pitch = pitch or 0.0
	self.Yaw = yaw or 0.0
	self.Roll = roll or 0.0
end
UE4.Rotator.ZeroRotator = UE4.Rotator.new(0.0, 0.0, 0.0)

function UE4.Rotator:GetForwardVector()
	return self:GetAxisX()
end

function UE4.Rotator:GetRightVector()
	return self:GetAxisY()
end

function UE4.Rotator:GetUpVector()
	return self:GetAxisZ()
end

--=============================================================================================================================
-- FQuat
--=============================================================================================================================
UE4.Quat = Class(PrimitiveType)
function UE4.Quat:init(x, y, z, w)
	self.X = x or 0.0
	self.Y = y or 0.0
	self.Z = z or 0.0
	self.W = w or 1.0
end
UE4.Quat.Identity = UE4.Quat.new(0.0, 0.0, 0.0, 1.0)

--=============================================================================================================================
-- FLinearColor
--=============================================================================================================================
UE4.LinearColor = Class(PrimitiveType)
function UE4.LinearColor:init(r, g, b, a)
	self.R = r or 0.0
	self.G = g or 0.0
	self.B = b or 0.0
	self.A = a or 1.0
end

--=============================================================================================================================
-- FColor
--=============================================================================================================================
UE4.Color = Class(PrimitiveType)
function UE4.Color:init(r, g, b, a)
	self.R = r or 0
	self.G = g or 0
	self.B = b or 0
	self.A = a or 0xFF
end

--=============================================================================================================================
-- FTransform
--=============================================================================================================================
UE4.Transform = Class(PrimitiveType)
function UE4.Transform:init(r, t, s)
	self.Rotation = r or UE4.Quat.Identity
	self.Translation = t or UE4.Vector.ZeroVector
	self.Scale3D = s or UE4.Vector.OneVector
end
UE4.Transform.Identity = UE4.Transform.new()

--=============================================================================================================================
-- FBox
--=============================================================================================================================
UE4.Box = Class(PrimitiveType)
function UE4.Box:init(min, max)
	self.Min = min or UE4.Vector.ZeroVector
	self.Max = max or UE4.Vector.ZeroVector
end

--=============================================================================================================================
-- FBox2D
--=============================================================================================================================
UE4.Box2D = Class(PrimitiveType)
function UE4.Box2D:init(min, max)
	self.Min = min or UE4.Vector2D.ZeroVector
	self.Max = max or UE4.Vector2D.ZeroVector
end

--=============================================================================================================================
-- FBoxSphereBounds
--=============================================================================================================================
UE4.BoxSphereBounds = Class(PrimitiveType)
function UE4.BoxSphereBounds:init(origin, extent, radius)
	self.Origin = origin or UE4.Vector.ZeroVector
	self.BoxExtent = extent or UE4.Vector.ZeroVector
	self.SphereRadius = radius or 0.0
end

--=============================================================================================================================
-- FGuid
--=============================================================================================================================
UE4.Guid = Class(PrimitiveType)
function UE4.Guid:init(a, b, c, d)
	self.A = a or 0
	self.B = b or 0
	self.C = c or 0
	self.D = d or 0
end

--=============================================================================================================================
-- FDateTime
--=============================================================================================================================
UE4.DateTime = Class(PrimitiveType)
function UE4.DateTime:init(ticks)
	self.Ticks = ticks or 0
end

--=============================================================================================================================
-- FTimecode
--=============================================================================================================================
UE4.Timecode = Class(PrimitiveType)
function UE4.Timecode:init(hours, minutes, seconds, frames, dropFrameFormat)
	self.Hours = hours or 0
	self.Minutes = minutes or 0
	self.Seconds = seconds or 0
	self.Frames = frames or 0
	self.bDropFrameFormat = dropFrameFormat or false
end

--=============================================================================================================================
-- FTimespan
--=============================================================================================================================
UE4.Timespan = Class(PrimitiveType)
function UE4.Timespan:init(ticks)
	self.Ticks = ticks or 0
end

--=============================================================================================================================
-- FRandomStream
--=============================================================================================================================
UE4.RandomStream = Class(PrimitiveType)
function UE4.RandomStream:init(seed)
	self.Seed = seed or 0
end

--=============================================================================================================================
-- FFrameRate
--=============================================================================================================================
UE4.FrameRate = Class(PrimitiveType)
function UE4.FrameRate:init(numerator, denominator)
	self.Numerator = numerator or 0
	self.Denominator = denominator or 0
end

--=============================================================================================================================
-- FFrameNumber
--=============================================================================================================================
UE4.FrameNumber = Class(PrimitiveType)
function UE4.FrameNumber:init(value)
	self.Value = value or 0
end

--=============================================================================================================================
-- FFrameTime
--=============================================================================================================================
UE4.FrameTime = Class(PrimitiveType)
function UE4.FrameTime:init(frame, subFrame)
	self.Frame = frame or UE4.FrameNumber.new()
	self.SubFrame = subFrame or 0
end

--=============================================================================================================================
-- FQualifiedFrameTime
--=============================================================================================================================
UE4.QualifiedFrameTime = Class(PrimitiveType)
function UE4.QualifiedFrameTime:init(time, rate)
	self.Time = time or UE4.FrameTime.new()
	self.Rate = rate or UE4.FrameRate.new()
end

--=============================================================================================================================
-- FPrimaryAssetType
--=============================================================================================================================
UE4.PrimaryAssetType = Class(PrimitiveType)
function UE4.PrimaryAssetType:init(name)
	self.Name = name or ""
end

--=============================================================================================================================
-- FPrimaryAssetId
--=============================================================================================================================
UE4.PrimaryAssetId = Class(PrimitiveType)
function UE4.PrimaryAssetId:init(primaryAssetType, primaryAssetName)
	self.PrimaryAssetType = primaryAssetType or UE4.PrimaryAssetType.new()
	self.PrimaryAssetName = primaryAssetName or ""
end

--=============================================================================================================================
-- FSoftObjectPath
--=============================================================================================================================
UE4.SoftObjectPath = Class(PrimitiveType)
function UE4.SoftObjectPath:init(assetPathName, subPathString)
	self.AssetPathName = assetPathName or ""
	self.SubPathString = subPathString or ""
end

--=============================================================================================================================
-- FFloatRangeBound
--=============================================================================================================================
UE4.FloatRangeBound = Class(PrimitiveType)
function UE4.FloatRangeBound:init(type, value)
	self.Type = type or 0
	self.Value = value or 0.0
end

--=============================================================================================================================
-- FFloatRange
--=============================================================================================================================
UE4.FloatRange = Class(PrimitiveType)
function UE4.FloatRange:init(lowerBound, upperBound)
	self.LowerBound = lowerBound or UE4.FloatRangeBound.new()
	self.UpperBound = upperBound or UE4.FloatRangeBound.new()
end

--=============================================================================================================================
-- FInt32Interval
--=============================================================================================================================
UE4.Int32Interval = Class(PrimitiveType)
function UE4.Int32Interval:init(min, max)
	self.Min = min or 0
	self.Max = max or 0
end

--=============================================================================================================================
-- Enum
--=============================================================================================================================
UE4.Enum = Class(PrimitiveType)

function UE4.Enum:ToString(enumValue)
	for k, v in pairs(self) do
		if v == enumValue then return k end
	end
end

--=============================================================================================================================
-- Math
--=============================================================================================================================
UE4.Math = {}

--=============================================================================================================================
--PointEvent
--=============================================================================================================================
UE4.PointerEvent = Class()

--=============================================================================================================================
-- TextFormatter
--=============================================================================================================================
UE4.TextFormatter = Class()