include "../../Tools/Premake5/premakeDefines.lua"

project "CommonTestUtils"
	kind "None"
	language "C++"

	SetupProject("CommonTestUtils")

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/vendor/gtest/googlemock/include",
		"%{wks.location}/Core",
		"%{wks.location}/Utils",
		"%{wks.location}/Physics",
		"%{wks.location}/%{IncludeDir.GLM}",
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.RTTR}",
		"%{wks.location}/%{IncludeDir.slot_map}",
		"%{wks.location}/Tests/CommonTestUtils",
	}
