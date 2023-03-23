#pragma once

#include <api/dto/config.hpp>

#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/component.hpp>
#include <oatpp/core/base/CommandLineArguments.hpp>

#include <api/token/jwt.hpp>

#include <cstdlib>

class AppComponent {
// Inner vars
private:
    // command line arguments 0_0
    oatpp::base::CommandLineArguments m_cmdArgs;

public:
    // Regular consturctor with cmd args
    AppComponent(const oatpp::base::CommandLineArguments& cmdArgs)
    : m_cmdArgs(cmdArgs)
    {}

public:
    // Reading config from config file
    // [this] for passing class fields
    OATPP_CREATE_COMPONENT(oatpp::Object<ConfigDto>, config)([this] {
        if (!m_cmdArgs.hasArgument("--config") 
        && (!m_cmdArgs.hasArgument("--host") || !(m_cmdArgs.hasArgument("port") || !m_cmdArgs.hasArgument("--pguri"))))
        {
            OATPP_LOGE("AppComponent", "Can't retrieve config.");
            //throw std::runtime_error("[AppComponent]: Can't retreive config");
        }

        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        
        // Making config actualy
        std::string cfgPath = m_cmdArgs.getNamedArgumentValue("--config", "");
        if (!cfgPath.empty()) {
            oatpp::String configText = oatpp::String::loadFromFile(cfgPath.c_str());

            auto parsed = objectMapper->readFromString<oatpp::Object<ConfigDto>>(configText);
            return parsed;
        }

        std::string host = m_cmdArgs.getNamedArgumentValue("--host", "0.0.0.0");
        std::string port = m_cmdArgs.getNamedArgumentValue("--port", "8080");

        //postgresql://[user[:password]@][netloc][:port][/dbname][?param1=value1&...]
        std::string pgUri = m_cmdArgs.getNamedArgumentValue("--pguri", "postgresql://videoserver:31337@192.168.0.6/videoserver");
        if (!pgUri.empty()) {
            auto config = ConfigDto::createShared();
            config->dbConnectionString = oatpp::String(pgUri.c_str(), pgUri.size());
            config->host = oatpp::String(host.c_str(), host.size());
            config->port = std::stoi(port);

            return config;
        }

        OATPP_LOGE("AppComponent", "--pguri not specified.");
        throw std::runtime_error("[AppComponent] --pguri not specified");
    }()); 

    OATPP_CREATE_COMPONENT(std::shared_ptr<JWT>, jwt)([]{
        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);

        oatpp::String secret = oatpp::String::loadFromFile(config->secretPath->c_str());
        if (secret == nullptr) throw std::runtime_error("secretPath is non-existing file!");
        
        return std::make_shared<JWT>(secret, config->issuer);
    }());
};