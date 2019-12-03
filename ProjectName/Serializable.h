#pragma once

#include "Typedefs.h"

namespace EssilorCO
{
	class Serializable
	{
	public:
		static InputDataMap DeserializeInputData(const boostfs::path& filename);
		static void SerializeOutputData(MesureOutputDataMap outputMap, boostfs::path outputDir);

		static MesureList DeserializeMesures(const boostfs::path& filename);//Get list of mesures names

		static LensParamList DeserializeLensParams(const boostfs::path& filename);
		static PhoneDataParamList DeserializePhoneParams(const boostfs::path& filename);
	private:
	};
}