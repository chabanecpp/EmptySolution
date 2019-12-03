#pragma once

#include <boost\filesystem.hpp>
#include <boost\optional.hpp>

namespace boostfs = boost::filesystem;

namespace EssilorCO
{
	enum CommandResult
	{
		LOAD_FROM_DATABASE = 0,
		SIMPLE_COMPUTE_RUN = 1,
		ERROR_IN_COMMAND_LINE = 2,
		UNHANDLED_EXCEPTION = 3,
	};

	class ICommand
	{
	public:
		enum class CommandType
		{
			CommandDataBase,
			CommandSimple,
			CommandUnknow,
		};
		CommandType GetType() const;
		virtual ~ICommand() {};
		
		bool SetOutputPath(const boostfs::path& p);

		const boostfs::path& GetOutputPath() const;

	protected:
		CommandType m_commandType = CommandType::CommandUnknow;
		boostfs::path outputPath;
	};

	//Command launch run from a dataBase of mesures
	class CommandDataBase : public ICommand
	{
	public:
		CommandDataBase()
			:ICommand()
		{
			m_commandType = CommandType::CommandDataBase;
		}

		bool SetDataBasePath(const boostfs::path& p);
		bool SetFileName(const boostfs::path& p);
		bool SetMesureFileName(const boostfs::path& p);

		const boostfs::path& GetDataBasePath() const;
		const boostfs::path& GetFileName() const;
		const boostfs::path& GetMesureFileName() const;
	private:
		boostfs::path databasePath;
		boostfs::path fileName;
		boostfs::path mesureFileName;
	};

	//Command simple launch for one mesure
	class CommandSimple : public ICommand
	{
	public:
		CommandSimple()
			:ICommand()
		{
			m_commandType = CommandType::CommandSimple;
		}

		bool SetJsonFilePath(const boostfs::path& p);


		const boostfs::path& GetJsonFilePath() const;
	private:
		boostfs::path m_jsonFilePath;
	};

	class CommandLine //Command line strategy class
	{
	public:
		CommandResult read(int argc, char** argv);
		boost::optional<const CommandDataBase&> GetCommandDataBase() const;
		boost::optional<const CommandSimple&> GetCommandSimple() const;
	private:
		CommandResult ReadSimple(int argc, char** argv);
		CommandResult ReadFromDataBase(int argc, char** argv);

		boost::optional<CommandDataBase&> CmdDataBase();
		boost::optional<CommandSimple&> CmdSimple();
		
		std::unique_ptr<ICommand> m_command = nullptr;
	};
}