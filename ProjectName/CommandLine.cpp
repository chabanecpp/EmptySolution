#include "CommandLine.h"

#include <boost\program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

namespace EssilorCO
{
	CommandResult CommandLine::read(int argc, char ** argv)
	{
		CommandResult ret = ReadSimple(argc, argv);

		if (ERROR_IN_COMMAND_LINE == ret)
		{
			ret = ReadFromDataBase(argc, argv);
			if (ERROR_IN_COMMAND_LINE == ret)
				ret = UNHANDLED_EXCEPTION;//Unknown command line
		}

		return ret;
	}

	boost::optional<const CommandDataBase&> CommandLine::GetCommandDataBase() const
	{
		if (nullptr != m_command && ICommand::CommandType::CommandDataBase == m_command.get()->GetType())
		{
			return *(dynamic_cast<const CommandDataBase*>(m_command.get()));
		}
		return boost::none;
	}

	boost::optional<const CommandSimple&> CommandLine::GetCommandSimple() const
	{
		if (nullptr != m_command && ICommand::CommandType::CommandSimple == m_command.get()->GetType())
		{
			return *(dynamic_cast<const CommandSimple*>(m_command.get()));
		}
		return boost::none;
	}

	CommandResult CommandLine::ReadSimple(int argc, char ** argv)
	{
		po::options_description desc("arguments");

		desc.add_options()
			("file", po::value<boostfs::path>()->required(), "input json file")
			("output", po::value<boostfs::path>()->required(), "output path");

		po::variables_map vm;
		try
		{
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm); // throws on error, so do after help in case 
							// there are any problems

			m_command.reset(new CommandSimple());

			if (CmdSimple().is_initialized())
			{
				CmdSimple().get().SetJsonFilePath(vm["file"].as<boostfs::path>());
				CmdSimple().get().SetOutputPath(vm["output"].as<boostfs::path>());
			}
		}
		catch (po::error & e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return ERROR_IN_COMMAND_LINE;
		}

		return CommandResult::SIMPLE_COMPUTE_RUN;
	}

	CommandResult CommandLine::ReadFromDataBase(int argc, char ** argv)
	{
		/** Define and parse the program options
		*/
		po::options_description desc("arguments");

		desc.add_options()
			("rootpath", po::value<boostfs::path>(), "root path (working directory)")
			("database", po::value<boostfs::path>()->required(), "database directory")
			("file", po::value<boostfs::path>()->required(), "input csv file")
			("output", po::value<boostfs::path>()->required(), "output path")
			("mesure", po::value<boostfs::path>(), "mesures file path");

		po::variables_map vm;
		try
		{
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm); // throws on error, so do after help in case 
							// there are any problems

			m_command.reset(new CommandDataBase());
			auto it = vm.find("rootpath");

			if (it != vm.end() && CmdDataBase().is_initialized() )
			{
				auto dbRelativePath = vm["database"].as<boostfs::path>();
				auto rootpath = vm["rootpath"].as<boostfs::path>();
				CmdDataBase().get().SetDataBasePath(rootpath /= dbRelativePath);

				auto fnRelativePath = vm["file"].as<boostfs::path>();
				rootpath = vm["rootpath"].as<boostfs::path>();
				CmdDataBase().get().SetFileName(rootpath /= fnRelativePath);

				auto outRelativePath = vm["output"].as<boostfs::path>();
				rootpath = vm["rootpath"].as<boostfs::path>();
				CmdDataBase().get().SetOutputPath(rootpath /= outRelativePath);

				auto it2 = vm.find("mesure");
				if (it2 != vm.end())
				{
					auto mesureRelativePath = vm["mesure"].as<boostfs::path>();
					rootpath = vm["rootpath"].as<boostfs::path>();
					CmdDataBase().get().SetMesureFileName(rootpath /= mesureRelativePath);
				}
			}
			else if (CmdDataBase().is_initialized())
			{
				CmdDataBase().get().SetDataBasePath(vm["database"].as<boostfs::path>());
				CmdDataBase().get().SetFileName(vm["file"].as<boostfs::path>());
				CmdDataBase().get().SetOutputPath(vm["output"].as<boostfs::path>());

				auto it2 = vm.find("mesure");
				if (it2 != vm.end())
				{
					CmdDataBase().get().SetMesureFileName(vm["mesure"].as<boostfs::path>());
				}
			}
		}
		catch (po::error& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return ERROR_IN_COMMAND_LINE;
		}
		return LOAD_FROM_DATABASE;
	}

	boost::optional<CommandDataBase&> CommandLine::CmdDataBase()
	{
		if (nullptr != m_command && ICommand::CommandType::CommandDataBase == m_command.get()->GetType())
		{
			return *(dynamic_cast<CommandDataBase*>(m_command.get()));
		}
		return boost::none;
	}

	boost::optional<CommandSimple&> CommandLine::CmdSimple()
	{
		if (nullptr != m_command && ICommand::CommandType::CommandSimple == m_command.get()->GetType())
		{
			return *(dynamic_cast<CommandSimple*>(m_command.get()));
		}
		return boost::none;
	}

	bool CommandDataBase::SetDataBasePath(const boostfs::path & p)
	{
		bool ret = boostfs::is_directory(p);
		ret &= !boostfs::is_empty(p);
		if (ret)
			databasePath = p;
		return ret;
	}

	bool CommandDataBase::SetFileName(const boostfs::path & p)
	{
		bool ret = boostfs::is_regular_file(p);
		if(ret)
			fileName = p;
		return ret;
	}

	bool ICommand::SetOutputPath(const boostfs::path & p)
	{
		//Create if not exists
		if (false == boostfs::exists(p))
			boostfs::create_directories(p);

		bool ret = boostfs::is_directory(p);
		if (ret)
			outputPath = p;
		return ret;
	}

	bool CommandDataBase::SetMesureFileName(const boostfs::path & p)
	{
		bool ret = boostfs::is_regular_file(p);
		if (ret)
			mesureFileName = p;
		return ret;
	}

	const boostfs::path & CommandDataBase::GetDataBasePath() const
	{
		return databasePath;
	}

	const boostfs::path & CommandDataBase::GetFileName() const
	{
		return fileName;
	}

	const boostfs::path & ICommand::GetOutputPath() const
	{
		return outputPath;
	}

	const boostfs::path & CommandDataBase::GetMesureFileName() const
	{
		return mesureFileName;
	}

	ICommand::CommandType ICommand::GetType() const
	{
		return m_commandType;
	}

	bool CommandSimple::SetJsonFilePath(const boostfs::path& p)
	{
		bool ret = boostfs::is_regular_file(p);
		if (ret)
			m_jsonFilePath = p;
		return ret;
	}

	const boostfs::path& CommandSimple::GetJsonFilePath() const
	{
		return m_jsonFilePath;
	}
}