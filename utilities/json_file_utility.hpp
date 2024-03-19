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
    virtual void ToJson(nlohmann::json& j) const = 0;

    /// @brief Deserialize the object from a JSON representation.
    /// @param j The JSON object from which the object will be deserialized.
    virtual void FromJson(const nlohmann::json& j) = 0;
};

/// @brief Utility class for loading and saving data to JSON files.
/// @tparam T The type of data structure to be handled by this utility.
template<typename T>
class JsonFileUtility
{
public:
    /// @brief Constructs a JsonFileUtility object with the specified file path.
    /// @param filePath The file path to the JSON file.
    JsonFileUtility(const std::string& filePath) : filePath(filePath)
    {
        static_assert(std::is_base_of_v<JsonType, T>, "Structure must derive from JsonType");
    }

    /// @brief Loads data from the JSON file specified in the constructor.
    /// @return True if data is loaded successfully, false otherwise.
    bool Load()
    {
        try {
            if (!std::filesystem::exists(filePath))
            {
                data = T{};
            }
            else
            {
                std::ifstream file_stream(filePath);
                if (!file_stream.is_open())
                {
                    throw std::runtime_error("Failed to open file for reading.");
                }
                nlohmann::json j;
                file_stream >> j;
                data.FromJson(j);
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error loading file: " << ex.what() << std::endl;
            return false;
        }

        return Save();
    }

    /// @brief Saves data to the JSON file specified in the constructor.
    /// @return True if data is saved successfully, false otherwise.
    bool Save()
    {
        try
        {
            if (filePath.empty())
            {
                throw std::runtime_error("File path is empty.");
            }

            std::filesystem::path dir_path = std::filesystem::path(filePath).parent_path();
            if (!std::filesystem::exists(dir_path))
            {
                std::filesystem::create_directories(dir_path);
            }

            std::ofstream file_stream(filePath);
            if (!file_stream.is_open())
            {
                throw std::runtime_error("Failed to open file for writing.");
            }
            nlohmann::json j;
            data.ToJson(j);    // Convert object to json
            file_stream << std::setw(4) << j << std::endl;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error saving file: " << ex.what() << std::endl;
            return false;
        }

        return true;
    }

    /// @brief Gets a const reference to the internal data.
    /// @return A const reference to the internal data structure.
    const T& GetData() const
    {
        return data;
    }

    T data;                 // A copy of instance data

private:
    std::string filePath;   // A copy of the passing in file path 
};
