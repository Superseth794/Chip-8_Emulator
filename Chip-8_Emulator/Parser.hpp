//
//  Parser.hpp
//  Chip-8_Emulator
//
//  Created by Jaraxus on 24/01/2020.
//

#ifndef Parser_hpp
#define Parser_hpp

# include <iostream>
# include <string>
# include <fstream>
# include <sstream>

namespace chp {

/**
 Class to represent a file parser
 */
class Parser {
public:
    /**
     \brief Default constructor
     */
    Parser() = default;
    
    /**
     \brief Default destructor
     \details Closes file parsed if needed
     */
    ~Parser() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }
    
    /**
     \brief Deleted move constructor
     */
    Parser(Parser && other) = delete;
    
    /**
     \brief Deleted copy constructor
     */
    Parser(Parser const& other) = delete;
    
    /**
     \brief Deleted assignement operator
     */
    void operator=(Parser const& other) = delete;
    
    
    /**
     \brief Opens file to parse
     \param filename String storing the name of file to open
     \return True if file could be opened
     */
    bool loadFile(std::string const& filename) {
        m_file.open(filename);
        m_fileName = filename;
        return m_file.is_open();
    }
    
    /**
     \brief Reloads parser in file
     \details Resets parsing position at begining
     */
    void reload() {
        m_file.clear();
        m_file.seekg(0, std::ios_base::beg);
    }
    
    
    /**
     \brief Gets value parsed at adentifier
     \details The value parsed must be located in patern <identifier>: <value to parse>
     \param identifier The identifier of the value to parse
     \tparam T The type of the value to parse
     \return The value parsed after identifier. If parsing gould not be donne or if identifier could not be found, std::nullopt is returned.
     */
    template <typename T>
    std::optional<T> get(std::string const& identifier) {
        auto subString = std::move(find(identifier));
        
        if (subString == "") {
            std::cout << "Error: could not parse data from identifier: " << identifier << std::endl;
            return std::nullopt;
        }
        
        T result;
        std::stringstream stream(subString);
        
        while (stream.get() != ':') {}
        stream.get();
        
        stream >> result;
        
        return result;
    }
    
    /**
    \brief Gets value parsed at adentifier
    \details The value parsed must be located in patern <identifier>: <value to parse>
    \details The value must be "true" or "false"
    \param identifier The identifier of the value to parse
    \return The value parsed after identifier. If parsing gould not be donne or if identifier could not be found, std::nullopt is returned.
    */
    template <>
    std::optional<bool> get<bool>(std::string const& identifier) {
        std::string subString = std::move(find(identifier));
        
        if (subString == "") {
            std::cout << "Error: could not parse data from identifier: " << identifier << std::endl;
            return std::nullopt;
        }

        std::string result;
        std::stringstream stream(subString);

        while (stream.get() != ':') {}
        stream.get();

        char read = stream.get();
        while (read != ' ' && !stream.eof()) {
             result += read;
             read = stream.get();
        }
        
        if (result == "true") {
            return std::optional<bool>(true);
        } else if (result == "false") {
            return std::optional<bool>(false);
        }
        
        return std::nullopt;
    }
    
private:
    /**
     \brief Finds row containing the given identifier
     \param identifier The identifier of the value to parse
     \return A string containing the all the line after the first found identifier. If no identifier could be found, a null string is returned.
     */
//    std::string&& find(std::string const& identifier) {
    std::string find(std::string const& identifier) {
        auto startIdentifier {m_file.tellg()};
        bool eofReached = false;
        std::string result;
        do {
            if (m_file.ios_base::eof()) {
                if (eofReached)
                    return "";
                reload();
                eofReached = true;
            }
            std::getline(m_file, result);
            auto pos {result.find(identifier)};
            if (pos != std::string::npos)
                return std::forward<std::string>(std::string(result, pos, std::string::npos));
        } while (m_file.tellg() != startIdentifier);
//        return std::forward<std::string>(result);
        return "";
    }
    
private:
    static constexpr std::size_t MAX_INPUT_SIZE = 2048;
    
    std::ifstream m_file;
    std::string m_fileName;
};

}

#endif /* Parser_hpp */
