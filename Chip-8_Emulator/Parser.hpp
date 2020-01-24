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

class Parser {
public:
    Parser() = default;
    ~Parser() {
        m_file.close();
    }
    
    Parser(Parser && other) = delete;
    Parser(Parser const& other) = delete;
    void operator=(Parser const& other) = delete;
    
    bool loadFile(std::string const& filename) {
        m_file.open(filename);
        m_fileName = filename;
        return m_file.is_open();
    }
    
    void reload() {
        m_file.seekg(std::ios_base::beg);
    }
    
    template <typename T>
    std::optional<T> get(std::string const& identifier) {
        auto subString = std::move(find(identifier));
        
        if (subString == "")
            return std::nullopt;
        
        T result;
        std::stringstream stream(subString);
        
        while (stream.get() != ':') {}
        std::cout << stream.get() << std::endl;
        
        stream >> result;
        
        return result;
    }
    
    template <>
    std::optional<bool> get<bool>(std::string const& identifier) {
        std::string subString = std::move(find(identifier));

        if (subString == "")
            return std::nullopt;

        std::string result;
        std::stringstream stream(subString);

        while (stream.get() != ':') {}
        std::cout << stream.get() << std::endl;

        char read = stream.get();
        while (read != ' ' && !stream.eof()) {
             result += read;
             read = stream.get();
        }
        
        std::cout << subString << std::endl;
        
        return std::optional<bool>(result == "true");
    }
    
private:
//    std::string&& find(std::string const& identifier) {
    std::string find(std::string const& identifier) {
        auto startIdentifier {m_file.tellg()};
        std::string result;
        do {
            if (m_file.ios_base::eof())
                reload();
            std::getline(m_file, result);
            if (result.find(identifier) != std::string::npos)
                return std::forward<std::string>(result);
        } while (m_file.tellg() != startIdentifier);
        result = "";
//        return std::forward<std::string>(result);
        return result;
    }
    
private:
    static constexpr std::size_t MAX_INPUT_SIZE = 2048;
    
    std::ifstream m_file;
    std::string m_fileName;
};

}

#endif /* Parser_hpp */
