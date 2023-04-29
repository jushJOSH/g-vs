#pragma once

#include <api/dto/config.hpp>

#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/component.hpp>
#include <oatpp/core/base/CommandLineArguments.hpp>

#include <api/token/jwt.hpp>
#include <video/videoserver/videoserver.hpp>

#include <cstdlib>
#include <filesystem>

class AppComponent {
// Inner vars
private:
    // command line arguments 0_0
    oatpp::base::CommandLineArguments m_cmdArgs;

public:
    // Regular consturctor with cmd args
    AppComponent(const oatpp::base::CommandLineArguments& cmdArgs)
    :   m_cmdArgs(cmdArgs)
    {}
    
public:
    // Create serializer/deserializer mapper
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)([] {
        auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        mapper->getSerializer()->getConfig()->useBeautifier = true;
        mapper->getSerializer()->getConfig()->escapeFlags = 0;
        return mapper;
    }());

    // Reading config from config file
    // [this] for passing class fields
    OATPP_CREATE_COMPONENT(oatpp::Object<ConfigDto>, config)([this] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);
        if (m_cmdArgs.hasArgument("--make-default")) {
            auto defaultCfg = ConfigDto::createShared();
            auto configString = objectMapper->writeToString(defaultCfg);
            configString.saveToFile("videoserver.config.default");
            OATPP_LOGI("App Component", "Created default config 'videoserver.config.default'");
        }
        
        // Making config actualy
        std::string cfgPath = m_cmdArgs.getNamedArgumentValue("--config", "");
        if (!cfgPath.empty()) {
            oatpp::String configText = oatpp::String::loadFromFile(cfgPath.c_str());

            auto parsed = objectMapper->readFromString<oatpp::Object<ConfigDto>>(configText);
            return parsed;
        }

        // Create new config
        auto config = ConfigDto::createShared();

        config->host = m_cmdArgs.getNamedArgumentValue("--host", "0.0.0.0");
        config->port = std::stoi(m_cmdArgs.getNamedArgumentValue("--port", "8080"));
        if (!m_cmdArgs.hasArgument("--pguri"))
            throw std::runtime_error("'--pguri' not specified!");
        config->dbConnectionString = m_cmdArgs.getNamedArgumentValue("--pguri", "");

        if (!m_cmdArgs.hasArgument("--cert") ||
            !m_cmdArgs.hasArgument("--key") || 
            !m_cmdArgs.hasArgument("--df"))
        {
            OATPP_LOGW("App Component", "Server will be started INSECURE mode! Specify path to cert, key and Diffie-Hellman parameters using '--cert', '--key', '--df'")
        } else {
            config->apiCert = m_cmdArgs.getNamedArgumentValue("--cert", "");
            config->apiDiffieHellman = m_cmdArgs.getNamedArgumentValue("--df", "");
            config->apiKey = m_cmdArgs.getNamedArgumentValue("--key", "");
        }

        if (!m_cmdArgs.hasArgument("--token-salt")) {
            OATPP_LOGE("App Component", "Not specified '--token-salt' parameter. Specify PATH to salt file. File can contain any information");
            throw std::runtime_error("No token salt in config");
        } else 
            config->tokenSalt = m_cmdArgs.getNamedArgumentValue("--token-salt", "");

        return config;
    }()); 

    OATPP_CREATE_COMPONENT(std::shared_ptr<JWT>, jwt)([]{
        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);

        oatpp::String secret = oatpp::String::loadFromFile(config->tokenSalt->c_str());
        if (secret == nullptr) throw std::runtime_error("secretPath is non-existing file!");
        
        return std::make_shared<JWT>(secret, config->issuer);
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<Videoserver>, videoserver)([]{
        std::shared_ptr<Videoserver> videoserver = std::make_shared<Videoserver>();
        videoserver->accelerator = Videoserver::Accelerator::CPU;        

        return videoserver;
    }());
};