# Documentation


## Registration
Due to heavy usage of clang tidy I higly recomend to wrap any RTTR registration block into those comments.

```cpp
// clang-format off
RTTR_REGISTRATION
{
...
}
// clang-format on
```

## Serialization

To compress XML data we have number of strategies. For some simple types we support inline serialization into an attribute or inner XML data.

```cpp
bool SerializeString(const glm::vec3& vec, std::string& ret)
{
	ret = "(" + std::to_string(vec.x) + "," + std::to_string(vec.y) + "," + std::to_string(vec.z) + ")";
	return true;
}

bool DeserializeString(const std::string& str, glm::vec3& vec)
{
	if (str[0] != '(' || str[str.length() - 1] != ')')
		return false;
	std::stringstream ss(str.substr(1, str.length() - 2));

	return DeserializeFloatList(ss, vec);
}

#include <Utils/Serialization/SerializationUtils.h>

// clang-format off
RTTR_REGISTRATION
{
	REGISTER_SERIALIZATION(glm::vec3);
}
// clang-format on
```

For often repeated default values we can also define default values. They won't be seraialzed and when deserialize the property will be set to given value.

```cpp
.property("Transofrm", &C_Trimesh::m_Transofrm)(REGISTER_DEFAULT_VALUE(glm::mat4(1.f)))
```

Sometimes we want to register some properties but not to serialize them when saving into XML. We allow such behaviour using metadata.

Example shows property used for GUI but it is being saved through other property.
```cpp
.property("Rotation", &C_Transformations::m_Rotation)
(
	rttr::policy::prop::as_reference_wrapper, 
	RegisterMetaclass<MetaGUI::Vec3>(), 
	RegisterMetamember<UI::Vec3::Name>("Rotation"),
	RegisterMetamember<SerializationCls::NoSerialize>(true)
)
```