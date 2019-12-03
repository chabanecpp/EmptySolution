#pragma once
#include "PhoneData.h"
#include "LogPolicy.h"
#include "MesureData.h"
#include "CalculationResults.h"

#include "rapidjson/document.h"

#include <boost\filesystem.hpp>


namespace EssilorCO
{
	enum class JsonParserResults
	{
		Corrupted,
		MissingKey,
		MissingPhoneID,
		Ok,
	};
	class JsonParser;

	class JsonParserHelper
	{
	public:
		static JsonParserResults CheckjsonFile(const boost::filesystem::path& jsonFilePath);
		static Mesure GenerateMesureName(const boost::filesystem::path& jsonFilePath);//Key-PhoneID-Time
	private:
		friend JsonParser;
		static rapidjson::Document Read(const boost::filesystem::path& jsonFilePath);
		static std::string readKey(const rapidjson::Document& doc, const char* key);
	};

	class JsonParser
	{
	public:
		explicit JsonParser(const Mesure& name);
		JsonParserResults Read(const boost::filesystem::path& jsonFilePath);
		JsonParserResults Write(const CalculationResults& results);

		const PhoneData& GetPhoneData() const;
		const MesureData& GetMesureData() const;
	    LogPolicy* GetLogPolicy() const;

	private:
		PhoneData m_PhoneData;
		MesureData m_MesureData;
		std::shared_ptr<LogPolicy> m_LogPolicy;

		rapidjson::Document m_JsonDoc;
	};
}