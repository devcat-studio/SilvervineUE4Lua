-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.


DefaultParameters = {}
DefaultParameterClass = Class()

-- USceneComponent
do
	DefaultParameters.SceneComponent = {}

	-- SetupAttachment
	DefaultParameters.SceneComponent.SetupAttachment = Class(DefaultParameterClass)
	DefaultParameters.SceneComponent.SetupAttachment.InSocketName = ""
end