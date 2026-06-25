#include <UtilsTestStdafx.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

#include <UtilsTest/Serialization/DummyStructs.h>

// clang-format off
RTTR_REGISTRATION
{
	rttr::registration::class_<GlmVec3>("GlmVec3")
		.constructor<>()
		.property("vecNoSerialize", &GlmVec3::vecNoSerialize)
		(
			Utils::Reflection::RegisterMetamember<Utils::Reflection::SerializationCls::NoSerialize>(true)
		)
		.property("vec", &GlmVec3::vec);

	rttr::registration::enumeration<TestEnum>("TestEnum")
		(
			rttr::value("Value1", TestEnum::Value1),
			rttr::value("Value2", TestEnum::Value2),
			rttr::value("Value3", TestEnum::Value3)
		);

	rttr::registration::class_<AtomicTypesArrayStruct>("AtomicTypesArrayStruct")
		.constructor<>()
		.property("intArray", &AtomicTypesArrayStruct::m_IntArray)
		.property("floatArray", &AtomicTypesArrayStruct::m_FloatArray)
		.property("doubleArray", &AtomicTypesArrayStruct::m_DoubleArray)
		.property("boolArray", &AtomicTypesArrayStruct::m_BoolArray)
		.property("stringArray", &AtomicTypesArrayStruct::m_StringArray)
		.property("enumArray", &AtomicTypesArrayStruct::m_EnumArray);

	rttr::registration::class_<SetTypesStruct>("SetTypesStruct")
		.constructor<>()
		.property("intSet", &SetTypesStruct::m_IntSet)
		.property("stringSet", &SetTypesStruct::m_StringSet)
		.property("enumSet", &SetTypesStruct::m_EnumSet);

	rttr::registration::class_<VectorOfFloats>("VectorOfFloats")
		.constructor<>()
		.property("FloatVec", &VectorOfFloats::m_FloatVec);
	rttr::registration::class_<ColourStruct>("ColourStruct")
		.constructor<>()
		.property("colour", &ColourStruct::colour);
	
	rttr::registration::class_<RefWrapperStruct>("RefWrapperStruct")
		.constructor<>()
		.property("int", &RefWrapperStruct::m_Int)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("float", &RefWrapperStruct::m_Float)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("double", &RefWrapperStruct::m_Double)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("bool", &RefWrapperStruct::m_Bool)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("char", &RefWrapperStruct::m_Char)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("uint", &RefWrapperStruct::m_UInt)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("long", &RefWrapperStruct::m_Long)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("short", &RefWrapperStruct::m_Short)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("string", &RefWrapperStruct::m_String)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("enum", &RefWrapperStruct::m_Enum)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("path", &RefWrapperStruct::m_Path)
			(
				rttr::policy::prop::as_reference_wrapper
			);
	
	rttr::registration::class_<AtomicTypesStruct>("AtomicTypesStruct")
		.constructor<>()
		.property("int", &AtomicTypesStruct::m_Int)
		.property("float", &AtomicTypesStruct::m_Float)
		.property("double", &AtomicTypesStruct::m_Double)
		.property("bool", &AtomicTypesStruct::m_Bool)
		.property("char", &AtomicTypesStruct::m_Char)
		.property("uint", &AtomicTypesStruct::m_UInt)
		.property("long", &AtomicTypesStruct::m_Long)
		.property("short", &AtomicTypesStruct::m_Short)
		.property("string", &AtomicTypesStruct::m_String)
		.property("enum", &AtomicTypesStruct::m_Enum)
		.property("path", &AtomicTypesStruct::m_Path);

	rttr::registration::class_<RefWrapperAtomicTypesArrayStruct>("RefWrapperAtomicTypesArrayStruct")
		.constructor<>()
		.property("intArray", &RefWrapperAtomicTypesArrayStruct::m_IntArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("floatArray", &RefWrapperAtomicTypesArrayStruct::m_FloatArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("doubleArray", &RefWrapperAtomicTypesArrayStruct::m_DoubleArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("boolArray", &RefWrapperAtomicTypesArrayStruct::m_BoolArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("stringArray", &RefWrapperAtomicTypesArrayStruct::m_StringArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("enumArray", &RefWrapperAtomicTypesArrayStruct::m_EnumArray)
			(
				rttr::policy::prop::as_reference_wrapper
			);

	rttr::registration::class_<MapStruct>("MapStruct")
		.constructor<>()
		.property("stringToStringMap", &MapStruct::m_StringToStringMap)
		.property("stringToPathMap", &MapStruct::m_StringToPathMap)
		.property("pathToStringMap", &MapStruct::m_PathToStringMap)
		.property("pathToPathMap", &MapStruct::m_PathToPathMap)
		.property("intToStringMap", &MapStruct::m_IntToStringMap)
		.property("stringToIntMap", &MapStruct::m_StringToIntMap)
		.property("floatToDoubleMap", &MapStruct::m_FloatToDoubleMap)
		.property("enumToStringMap", &MapStruct::m_EnumToStringMap)
		.property("stringToEnumMap", &MapStruct::m_StringToEnumMap)
		.property("intToEnumMap", &MapStruct::m_IntToEnumMap);

	rttr::registration::class_<DefaultValueStruct>("DefaultValueStruct")
		.constructor<>()
		.property("wrongBool", &DefaultValueStruct::WrongBool)(REGISTER_DEFAULT_VALUE(true))
		;
}
// clang-format on
