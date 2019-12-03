// LensCopy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommandLine.h"
#include "LensCopyLoader.h"
#include "JsonParser.h"
#include "LensCopyInterface.h"

using namespace EssilorCO;

enum LensCopyResult
{
	SUCCESS = 0,
	COMMAND_ERROR = 1,
	LOAD_FAIL = 2,
	COMPUTE_FAIL = 3,
	UNHANDLED_EXCEPTION = 4,
};

int main(int argc, char** argv)
{
	CommandLine cmd;
	auto result = cmd.read(argc, argv);
	if (CommandResult::LOAD_FROM_DATABASE == result)
	{
		auto command = cmd.GetCommandDataBase();
		if (false == command.is_initialized())
			return LensCopyResult::COMMAND_ERROR;

		if (LoaderResult::success != LensCopyLoader::LoadPhoneData(command.get().GetFileName().parent_path()))
			return LensCopyResult::LOAD_FAIL;

		if (LoaderResult::success != LensCopyLoader::LoadLenses(command.get().GetFileName().parent_path()))
			return LensCopyResult::LOAD_FAIL;

		if (LoaderResult::success != LensCopyLoader::LoadInputData(command.get().GetFileName()))
			return LensCopyResult::LOAD_FAIL;

		auto computeList = LoaderHelper::GetMesuresToCompute(command.get().GetDataBasePath(), command.get().GetMesureFileName());
		if (computeList.empty())
			return LensCopyResult::LOAD_FAIL;

		if (false == LoaderHelper::Compute(computeList, command.get().GetOutputPath()))
			return LensCopyResult::COMPUTE_FAIL;
	}
	else if (CommandResult::SIMPLE_COMPUTE_RUN == result)
	{
		auto jsonFile = cmd.GetCommandSimple().get().GetJsonFilePath();
		
		auto result = JsonParserHelper::CheckjsonFile(jsonFile);

		if (JsonParserResults::Corrupted == result)
			return LensCopyResult::LOAD_FAIL;

		if (JsonParserResults::MissingKey != result && JsonParserResults::MissingPhoneID != result)
		{
			JsonParser parser(JsonParserHelper::GenerateMesureName(jsonFile));

			auto results = parser.Read(jsonFile);

			if (JsonParserResults::Ok == results)
			{
				LensCopyInterface lensCopy(parser.GetPhoneData(), parser.GetLogPolicy());
				auto calculation = lensCopy.Run(parser.GetMesureData(), cmd.GetCommandSimple().get().GetOutputPath().string() );

				auto writeResult = parser.Write(calculation);
			}
			else
				return LensCopyResult::LOAD_FAIL;
		}
		else
			return LensCopyResult::LOAD_FAIL;

	}
	else
	{
		return LensCopyResult::COMMAND_ERROR;
	}

	return LensCopyResult::SUCCESS;
}

