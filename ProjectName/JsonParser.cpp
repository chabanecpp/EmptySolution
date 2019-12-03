#pragma once
#include "JsonParser.h"
#include "LogPolicy.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <algorithm> 
#include <iterator>
#include <fstream>
#include <sstream>

#include <boost/date_time.hpp>

using namespace rapidjson;

namespace EssilorCO
{
	const char* KEY = "Key";
	const char* PHONEID = "PhoneId";
	const char* CAMERA_MATRIX = "CameraMatrix";
	const char* DISTORSION_COEFF = "DistortionCoefficient";
	const char* RESOLUTION = "Resolution";
	const char* SCREEN_SIZE = "ScreenSize";
	const char* SCREEN_DISTANCE = "ScreenDistance";
	const char* LOWER_POINTS = "LowerPoints";
	const char* ELLIPSE_1 = "ParamEllipse1";
	const char* ELLIPSE_2 = "ParamEllipse2";
	const char* OBJ_POINTS_1 = "ObjectPoints1";
	const char* OBJ_POINTS_2 = "ObjectPoints2";
	const char* MIRROR_THICKNESS = "MirrorThickness";
	const char* DEBUG_LEVEL = "DebugLevel";
	const char* EXTRINSIC_PARAM = "ExtrinsicParameters";

	JsonParser::JsonParser(const Mesure& name)
		:m_MesureData(name)
	{
	}
	JsonParserResults JsonParser::Read(const boost::filesystem::path& jsonFilePath)
	{
		auto ret = JsonParserHelper::CheckjsonFile(jsonFilePath);

		if(JsonParserResults::Ok == ret)
		{
			m_JsonDoc = JsonParserHelper::Read(jsonFilePath);
			//"Key":"KEY"

			//"PhoneId":"iPhone7+"

			//"CameraMatrix": {"fx":2889.103027,"fy":2887.386719,"cx":1153.727173,"cy":1522.934204}
			if(m_JsonDoc.HasMember(CAMERA_MATRIX) && m_JsonDoc[CAMERA_MATRIX].IsObject())
			{
				std::vector<real_t> vect;
				vect.push_back(m_JsonDoc[CAMERA_MATRIX].GetObject()["fx"].GetDouble());
				vect.push_back(m_JsonDoc[CAMERA_MATRIX].GetObject()["fy"].GetDouble());
				vect.push_back(m_JsonDoc[CAMERA_MATRIX].GetObject()["cx"].GetDouble());
				vect.push_back(m_JsonDoc[CAMERA_MATRIX].GetObject()["cy"].GetDouble());

				m_PhoneData.SetMatrix({ vect[0], 0.0, 0.0, 0.0, vect[1], 0.0, vect[2], vect[3], 1.0 });
			}

			//"DistortionCoefficient":{"k1":0.074844,"k2":0.282073,"k3":-1.753285,"p1":-0.002202,"p2":-0.000960}
			{
				m_PhoneData.distortionCoeffs.push_back(m_JsonDoc[DISTORSION_COEFF].GetObject()["k1"].GetDouble());
				m_PhoneData.distortionCoeffs.push_back(m_JsonDoc[DISTORSION_COEFF].GetObject()["k2"].GetDouble());
				m_PhoneData.distortionCoeffs.push_back(m_JsonDoc[DISTORSION_COEFF].GetObject()["k3"].GetDouble());
				m_PhoneData.distortionCoeffs.push_back(m_JsonDoc[DISTORSION_COEFF].GetObject()["p1"].GetDouble());
				m_PhoneData.distortionCoeffs.push_back(m_JsonDoc[DISTORSION_COEFF].GetObject()["p2"].GetDouble());
			}

			//"Resolution":{"Horizontal":1080.000000,"Vertical":1920.000000},
			{
				m_PhoneData.HRes = m_JsonDoc[RESOLUTION].GetObject()["Horizontal"].GetDouble();
				m_PhoneData.VRes = m_JsonDoc[RESOLUTION].GetObject()["Vertical"].GetDouble();
			}

			//"ScreenSize":{"Width":68.360001,"Height":121.540001}
			{
				m_PhoneData.screenWidth = m_JsonDoc[SCREEN_SIZE].GetObject()["Width"].GetDouble();
				m_PhoneData.screenHeigth = m_JsonDoc[SCREEN_SIZE].GetObject()["Height"].GetDouble();
			}

			//"ScreenDistance":{"Horizontal":20.740000,"Vertical":8.660000}
			{
				m_PhoneData.ULScreenHDistance = m_JsonDoc[SCREEN_DISTANCE].GetObject()["Horizontal"].GetDouble();
				m_PhoneData.ULScreenVDistance = m_JsonDoc[SCREEN_DISTANCE].GetObject()["Vertical"].GetDouble();
			}

			//"ExtrinsicParameters": {"x": 0, "y":0}
			{
				m_PhoneData.cameraScreenXRotAngle = m_JsonDoc[EXTRINSIC_PARAM].GetObject()["x"].GetDouble();
				m_PhoneData.cameraScreenYRotAngle = m_JsonDoc[EXTRINSIC_PARAM].GetObject()["y"].GetDouble();
			}

			//"LowerPoints"
			if(m_JsonDoc[LOWER_POINTS].IsArray())
			{
				PixelVector LowerPoints;
			    auto a = m_JsonDoc[LOWER_POINTS].GetArray();

				for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
				{
					auto sub = itr->GetArray();
					Pixel p;
					for (Value::ConstValueIterator itrSub = sub.Begin(); itrSub != sub.End(); ++itrSub)
					{
						p.push_back(itrSub->GetDouble());
					}
					LowerPoints.push_back(p);
				}
				m_MesureData.SetDirectData(LowerPoints);
			}

			//"ParamEllipse1":{"CenterX":1175.302124,"CenterY":1876.498779,"MajorAxis":83.438522,"MinorAxis":82.824211,"Angle":177.905975}
			if (m_JsonDoc.HasMember(ELLIPSE_1) && m_JsonDoc[ELLIPSE_1].IsObject())
			{
				std::vector<real_t> vect;
				vect.push_back(m_JsonDoc[ELLIPSE_1].GetObject()["CenterX"].GetDouble());
				vect.push_back(m_JsonDoc[ELLIPSE_1].GetObject()["CenterY"].GetDouble());
				vect.push_back(m_JsonDoc[ELLIPSE_1].GetObject()["MajorAxis"].GetDouble());
				vect.push_back(m_JsonDoc[ELLIPSE_1].GetObject()["MinorAxis"].GetDouble());
				vect.push_back(m_JsonDoc[ELLIPSE_1].GetObject()["Angle"].GetDouble());

				m_MesureData.parmsEllipse1 = vect;
			}

			//"ParamEllipse2":{"CenterX":1175.302124,"CenterY":1876.498779,"MajorAxis":83.438522,"MinorAxis":82.824211,"Angle":177.905975}
			if (m_JsonDoc.HasMember(ELLIPSE_2) && m_JsonDoc[ELLIPSE_2].IsObject())
			{
				std::vector<real_t> vect;
				vect.push_back(m_JsonDoc[ELLIPSE_2].GetObject()["CenterX"].GetDouble());
				vect.push_back(m_JsonDoc[ELLIPSE_2].GetObject()["CenterY"].GetDouble());
				vect.push_back(m_JsonDoc[ELLIPSE_2].GetObject()["MajorAxis"].GetDouble());
				vect.push_back(m_JsonDoc[ELLIPSE_2].GetObject()["MinorAxis"].GetDouble());
				vect.push_back(m_JsonDoc[ELLIPSE_2].GetObject()["Angle"].GetDouble());

				m_MesureData.parmsEllipse2 = vect;
			}

			//"ObjectPoints1": {}
			if (m_JsonDoc[OBJ_POINTS_1].IsArray())
			{
				PixelVector pixel;
				auto a = m_JsonDoc[OBJ_POINTS_1].GetArray();

				for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
				{
					auto sub = itr->GetArray();
					Pixel p;
					for (Value::ConstValueIterator itrSub = sub.Begin(); itrSub != sub.End(); ++itrSub)
					{
						p.push_back(itrSub->GetDouble());
					}
					pixel.push_back(p);
				}
				m_MesureData.circleImagePixels1 = pixel;
				m_MesureData.circleSourcePixels1 = pixel;
			}


			//"ObjectPoints2": {}
			if (m_JsonDoc[OBJ_POINTS_2].IsArray())
			{
				PixelVector pixel;
				auto a = m_JsonDoc[OBJ_POINTS_2].GetArray();

				for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
				{
					auto sub = itr->GetArray();
					Pixel p;
					for (Value::ConstValueIterator itrSub = sub.Begin(); itrSub != sub.End(); ++itrSub)
					{
						p.push_back(itrSub->GetDouble());
					}
					pixel.push_back(p);
				}
				m_MesureData.circleImagePixels2 = pixel;
				m_MesureData.circleSourcePixels2 = pixel;
			}
		}

		//"DebugLevel":"Debug",
		{
			auto logLevel = StringToErrorStatus(m_JsonDoc[DEBUG_LEVEL].GetString());
			m_LogPolicy = std::make_shared<DefaultLogPolicy>("C:\\Users\\chabanew\\Documents\\Data", m_MesureData.m_Name, logLevel );
        }

		return ret;
	}
	JsonParserResults JsonParser::Write(const CalculationResults& results)
	{
		//"LensRX": {"Sphere" = 2.23, "Cylinder" = 0.002, "Axis" = 3.23 },
		Value obj(kObjectType);
		{
			Value sphere(kNumberType);
			Value cylinder(kNumberType);
			Value axis(kNumberType);

			sphere.SetDouble(results.Prescription.Sphere);
			cylinder.SetDouble(results.Prescription.Cylinder);
			axis.SetDouble(results.Prescription.Axis);

			obj.AddMember("Sphere", sphere, m_JsonDoc.GetAllocator());
			obj.AddMember("Cylinder", cylinder, m_JsonDoc.GetAllocator());
			obj.AddMember("Axis", axis, m_JsonDoc.GetAllocator());
		}
		m_JsonDoc.AddMember("LensRX", obj, m_JsonDoc.GetAllocator());

		//"Mesure": "KEYiPhone7+2019-Dec-03 10:27:37"
		Value str(kStringType);
		str.SetString(m_MesureData.m_Name.c_str(), m_MesureData.m_Name.length());
		m_JsonDoc.AddMember("Mesure", str, m_JsonDoc.GetAllocator());

		//"Log" : "log_LogFile.log"
		Value strLog(kStringType);
		auto logLevel = LogLevelToString(m_LogPolicy->ReportingLevel());
		strLog.SetString(logLevel.c_str(), logLevel.length());
		m_JsonDoc.AddMember("Log", strLog, m_JsonDoc.GetAllocator());

		//Write to a file
		{
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			m_JsonDoc.Accept(writer);

			auto str = buffer.GetString();
		}
		return JsonParserResults::Ok;
	}
	const PhoneData& JsonParser::GetPhoneData() const
	{
		return m_PhoneData;
	}
	const MesureData& JsonParser::GetMesureData() const
	{
		return m_MesureData;
	}
	LogPolicy* JsonParser::GetLogPolicy() const
	{
		return m_LogPolicy.get();
	}
	JsonParserResults JsonParserHelper::CheckjsonFile(const boost::filesystem::path& jsonFilePath)
	{
		auto doc = Read(jsonFilePath);

		if (doc.HasParseError())
			return JsonParserResults::Corrupted;
		else
		{
			if (false == doc.HasMember(KEY))
				return JsonParserResults::MissingKey;
			else if (false == doc.HasMember(PHONEID))
				return JsonParserResults::MissingPhoneID;
			else
				return JsonParserResults::Ok;
		}
	}
	Mesure JsonParserHelper::GenerateMesureName(const boost::filesystem::path& jsonFilePath)
	{
		auto doc = Read(jsonFilePath);
		
		boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();

		Mesure name = readKey(doc, KEY) + readKey(doc, PHONEID) /*+ boost::posix_time::to_simple_string(timeLocal)*/;

		return name;
	}
	rapidjson::Document JsonParserHelper::Read(const boost::filesystem::path& jsonFilePath)
	{
		rapidjson::Document ret;

		std::ifstream file(jsonFilePath.string());
		if (file.is_open())
		{
			std::ostringstream sout;
			std::copy(std::istreambuf_iterator<char>(file),
				std::istreambuf_iterator<char>(),
				std::ostreambuf_iterator<char>(sout));

			std::string stringbuffer = sout.str().c_str();
			ret.Parse(sout.str().c_str());
			file.close();
		}
		return ret;
	}
	std::string JsonParserHelper::readKey(const rapidjson::Document& doc, const char* key)
	{
		std::string ret;

		if (doc.HasMember(key) && doc[key].IsString())
			ret = std::string(doc[key].GetString());

		return ret;
	}
}
