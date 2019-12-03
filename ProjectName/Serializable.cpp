#include "Serializable.h"

#include <boost\tokenizer.hpp>
#include <boost\algorithm\string.hpp>

namespace boostAlgo = boost::algorithm;

namespace EssilorCO
{
	namespace SerializableHelper
	{
		typedef std::string Line;
		typedef std::vector<std::string> Lines;

		static Lines GetLines(const boostfs::path & filename)
		{
			Lines lines;
			boostfs::ifstream file(filename);
			Line line;
			while (std::getline(file, line))
			{
				boostAlgo::trim(line);
				if(false == line.empty())
					lines.push_back(line);
			}
			return lines;
		}

		static bool CheckInputFile(Line firstLine)
		{
			bool bRet(true);
			typedef boost::tokenizer<boost::char_separator<char> >
				tokenizer;

			boost::char_separator<char> sep(";");
			tokenizer tok(firstLine, sep);

			int column = 0;
			for (tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg)
			{
				std::string trimmedToken = *beg;
				boostAlgo::trim(trimmedToken);
				if (0 == column)
				{
					if (trimmedToken != "Measure")
					{
						bRet = false;
						break;
					}
				}
				else
				{
					auto tokenOrder = static_cast<int>(InputDataHelper::FromString(trimmedToken)) + 1;
					if( tokenOrder != column)
					{
						bRet = false;
						break;
					}
				}
				column++;
			}
			return bRet;
		}
	};

	InputDataMap Serializable::DeserializeInputData(const boostfs::path & filename)
	{
		InputDataMap ret;

		InputData inputData;

		auto lines = SerializableHelper::GetLines(filename);

		//Parse one line
		int i = 0;
		for (auto line : lines)
		{
			//check the first line
			if (0 == i)
			{
				if (false == SerializableHelper::CheckInputFile(line))
					break;
			}
			else
			{
				typedef boost::tokenizer<boost::char_separator<char> >
					tokenizer;

				boost::char_separator<char> sep(";");
				tokenizer tok(line, sep);

				int column = 0;
				Mesure mesure;
				for (tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg)
				{
					std::string trimmedToken = *beg;
					boostAlgo::trim(trimmedToken);

					switch (column)
					{
					case 0://Measure
					{
						mesure = trimmedToken;
						break;
					}
					case 1://Lens
					{
						inputData.Lens = trimmedToken;
						break;
					}
					case 2://Laterality
					{
						inputData.Laterality = trimmedToken;
						break;
					}
					case 3://PhoneData
					{
						inputData.phoneData = trimmedToken;
						break;
					}
					case 4://Fronto_Sph
					{
						inputData.Fronto_Sph = std::stod(trimmedToken);
						break;
					}
					case 5://Fronto_Cyl
					{
						inputData.Fronto_Cyl = std::stod(trimmedToken);
						break;
					}
					case 6://Fronto_Ax
					{
						inputData.Fronto_Ax = std::stod(trimmedToken);
						break;
					}
					case 7://Fronto_Ax
					{
						inputData.CO_HA = trimmedToken;
						break;
					}
					default:
						break;
					}
					column++;
				}

				ret.insert(std::make_pair(mesure, inputData));
			}

			i++;
		}
		return ret;
	}

	void Serializable::SerializeOutputData(MesureOutputDataMap outputMap, boostfs::path outputDir)
	{
		boostfs::path filename = outputDir.append(outputDir.leaf().string() + std::string(".csv"));
		std::ofstream file(filename.string());
		std::string sep(" ; ");
		{
			//Measure 	Lens 	 Fronto_Sph 	Fronto_Cyl 	Fronto_Ax 	Solution_Sph	 Solution_Cyl 	 Solution_Axe 	 ErrSph 	 ErrCyl 	 ErrAxe	Tx	Ty	Tz	Rx	Ry	PositioningError ReconstructionError	Status	Message
			file << "Measure" << sep << "Lens" << sep << "Fronto_Sph" << sep << "Fronto_Cyl" << sep << "Fronto_Ax" << sep << "Solution_Sph" << sep << "Solution_Cyl" << sep << "Solution_Axe" << sep << "ErrSph" << sep << "ErrCyl" << sep << "ErrAxe" << sep << "Tx" << sep << "Ty" << sep << "Tz" << sep << "Rx" << sep << "Ry" << sep << "PositioningError" << sep << "ReconstructionError" << sep << "Status" << sep << "Message";
		}

		for (auto item : outputMap)
		{
			std::string inputs = item.first + sep + item.second.input.Lens + sep + std::to_string(item.second.input.Fronto_Sph) + sep + std::to_string(item.second.input.Fronto_Cyl) + sep + std::to_string(item.second.input.Fronto_Ax);
			std::string outpts = std::to_string(item.second.calculationResults.Prescription.Sphere) + sep + std::to_string(item.second.calculationResults.Prescription.Cylinder) + sep + std::to_string(item.second.calculationResults.Prescription.Axis) + sep + std::to_string(item.second.ErrSph) + sep + std::to_string(item.second.ErrCyl) + sep + std::to_string(item.second.ErrAxe);
			std::string outpts2 = std::to_string(item.second.calculationResults.Tx) + sep + std::to_string(item.second.calculationResults.Ty) + sep + std::to_string(item.second.calculationResults.Tz) + sep + std::to_string(item.second.calculationResults.Rx) + sep + std::to_string(item.second.calculationResults.Ry) + sep + std::to_string(item.second.calculationResults.PositioningError) + sep + std::to_string(item.second.calculationResults.ReconstructionError) + sep + ErrorStatusToString(item.second.calculationResults.Status) + sep + item.second.calculationResults.Message;

			file << std::endl << inputs << sep << outpts << sep << outpts2;
		}
	}

	MesureList Serializable::DeserializeMesures(const boostfs::path & filename)
	{
		MesureList ret;

		auto lines = SerializableHelper::GetLines(filename);

		for (auto line : lines)
		{
			typedef boost::tokenizer<boost::char_separator<char> >
				tokenizer;

			boost::char_separator<char> sep{ ";", "+", boost::keep_empty_tokens };
			tokenizer tok(line, sep);

			for (tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg)
			{
				std::string trimmedToken = *beg;
				boostAlgo::trim(trimmedToken);
				if(false == trimmedToken.empty())
					ret.push_back(trimmedToken);
			}
		}
		return ret;
	}

	LensParamList Serializable::DeserializeLensParams(const boostfs::path & filename)
	{
		LensParamList ret;

		auto lines = SerializableHelper::GetLines(filename);

		typedef boost::tokenizer<boost::char_separator<char>>
			tokenizer;
		boost::char_separator<char> sep(";");

		//Parse one line
		int i = 0;
		for (auto line : lines)
		{
			//ignore the first line
			if (0 != i)
			{
				tokenizer tok(line, sep);

				LensParam lens;
				int column = 0;
				for (tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg)
				{
					std::string trimmedToken = *beg;
					boostAlgo::trim(trimmedToken);

					switch (column)
					{
					case 0://Lens name
					{
						lens.name = trimmedToken;
						break;
					}
					case 1://laterality
					{
						lens.laterality = FromString(trimmedToken);
						break;
					}
					case 2://range
					{
						lens.range = std::stod(trimmedToken);
						break;
					}
					case 3://Fronto_Sph
					{
						lens.fronto_Sph = std::stod(trimmedToken);
						break;
					}
					case 4://Fronto_Cyl
					{
						lens.fronto_Cyl = std::stod(trimmedToken);
						break;
					}
					case 5://Fronto_Ax
					{
						lens.fronto_Ax = std::stod(trimmedToken);
						break;
					}
					default:
						break;
					}
					column++;
				}

				ret.push_back(lens);
			}

			i++;
		}
		return ret;
	}

	PhoneDataParamList Serializable::DeserializePhoneParams(const boostfs::path & filename)
	{
		PhoneDataParamList ret;

		auto lines = SerializableHelper::GetLines(filename);

		typedef boost::tokenizer<boost::char_separator<char>>
			tokenizer;
		boost::char_separator<char> sep(";");

		//Parse one line
		int i = 0;
		for (auto line : lines)
		{
			//ignore the first line
			if (0 != i)
			{
				tokenizer tok(line, sep);

				PhoneDataParam phoneData;
				double m1{0}, m2{0}, m3{0}, m4{0};
				int column = 0;
				for (tokenizer::iterator beg = tok.begin(); beg != tok.end(); ++beg)
				{
					std::string trimmedToken = *beg;
					boostAlgo::trim(trimmedToken);

					switch (column)
					{
					case 0://Name
					{
						phoneData.name = trimmedToken;
						break;
					}
					case 1://HRes
					{
						phoneData.HRes = std::stoi(trimmedToken);
						break;
					}
					case 2://VRes
					{
						phoneData.VRes = std::stoi(trimmedToken);
						break;
					}
					case 3://screenWidth
					{
						phoneData.screenWidth = std::stod(trimmedToken);
						break;
					}
					case 4://screenHeigth
					{
						phoneData.screenHeigth = std::stod(trimmedToken);
						break;
					}
					case 5://ULScreenVDistance
					{
						phoneData.ULScreenVDistance = std::stod(trimmedToken);
						break;
					}
					case 6://ULScreenHDistance
					{
						phoneData.ULScreenHDistance = std::stod(trimmedToken);
						break;
					}
					case 7://k1
					{
						phoneData.k1 = std::stod(trimmedToken);
						break;
					}
					case 8://k2
					{
						phoneData.k2 = std::stod(trimmedToken);
						break;
					}
					case 9://k3
					{
						phoneData.k3 = std::stod(trimmedToken);
						break;
					}
					case 10://p1
					{
						phoneData.p1 = std::stod(trimmedToken);
						break;
					}
					case 11://p2
					{
						phoneData.p2 = std::stod(trimmedToken);
						break;
					}
					case 12://matrix1
					{
						m1 = std::stod(trimmedToken);
						break;
					}
					case 13://matrix2
					{
						m2 = std::stod(trimmedToken);
						break;
					}
					case 14://matrix3
					{
						m3 = std::stod(trimmedToken);
						break;
					}
					case 15://matrix4
					{
						m4 = std::stod(trimmedToken);
						break;
					}
					default:
						break;
					}
					column++;
				}

				phoneData.SetMatrix(m1, m2, m3, m4);
				ret.push_back(phoneData);
			}

			i++;
		}
		return ret;
	}
}