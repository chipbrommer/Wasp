#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            json_file_utility.h
// @brief           Class to handle interfacing with a json file
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <iostream>                         // console io
#include <fstream>                          // file io
#include <exception>                        // exceptions
#include <filesystem>                       // files
//
#include "../external/nlohmann/json.hpp"    // json
//
/////////////////////////////////////////////////////////////////////////////////

/// @brief Base Json File Type that any structure being handled must derive from.
class JsonType
{
public:

    /// @brief Serialize the object to a JSON representation.
    /// @param j The JSON object to which the object will be serialized.
    virtual void to_json(nlohmann::json& j) const = 0;

    /// @brief Deserialize the object from a JSON representation.
    /// @param j The JSON object from which the object will be deserialized.
    virtual void from_json(const nlohmann::json& j) = 0;
};

template<typename T>
class JsonFileUtility 
{
public:
    JsonFileUtility(const std::string& file_path) : file_path(file_path) 
    {
        static_assert(std::is_base_of_v<JsonType, T>, "Structure must derive from JsonType");
    }

    bool Load() 
    {
        try {
            if (!std::filesystem::exists(file_path)) 
            {
                data = T{};
            }
            else 
            {
                std::ifstream file_stream(file_path);
                if (!file_stream.is_open()) 
                {
                    throw std::runtime_error("Failed to open file for reading.");
                }
                nlohmann::json j;
                file_stream >> j;
                data.from_json(j);
            }
        }
        catch (const std::exception& ex) 
        {
            std::cerr << "Error loading file: " << ex.what() << std::endl;
            return false;
        }

        return Save();
    }

    bool Save() 
    {
        try 
        {
            if (file_path.empty()) 
            {
                throw std::runtime_error("File path is empty.");
            }

            std::filesystem::path dir_path = std::filesystem::path(file_path).parent_path();
            if (!std::filesystem::exists(dir_path)) 
            {
                std::filesystem::create_directories(dir_path);
            }

            std::ofstream file_stream(file_path);
            if (!file_stream.is_open()) 
            {
                throw std::runtime_error("Failed to open file for writing.");
            }
            nlohmann::json j;
            data.to_json(j);    // Convert object to json
            file_stream << std::setw(4) << j << std::endl;
        }
        catch (const std::exception& ex) 
        {
            std::cerr << "Error saving file: " << ex.what() << std::endl;
            return false;
        }

        return true;
    }

    const T& GetData() const 
    {
        return data;
    }

private:
    T data;
    std::string file_path;
};
