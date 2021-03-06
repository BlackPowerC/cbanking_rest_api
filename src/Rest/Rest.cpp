//
// Created by jordy on 10/03/18.
//

#include "../../include/Rest/Rest.hpp"

namespace RestAPI
{
    void RestServer::routing()
    {
        /* Les routes GET */
        Rest::Routes::Get(this->t_router, "/customer/accounts/get/:token",
                          Rest::Routes::bind(&RequestHandler::getAccountByCustomerId, &this->t_rhandler)) ;
        // La liste de compte
        Rest::Routes::Get(this->t_router, "/account/get/all/:token",
                          Rest::Routes::bind(&RequestHandler::getAllAccounts, &this->t_rhandler)) ;
        // Un compte particulier
        Rest::Routes::Get(this->t_router,"/account/get/:id",
                          Rest::Routes::bind(&RequestHandler::getAccountById, &this->t_rhandler)) ;
        // Les comptes d'un client
        Rest::Routes::Get(this->t_router,"/customer/account/get/:id",
                          Rest::Routes::bind(&RequestHandler::getAccountByCustomerId, &this->t_rhandler)) ;
        // Les comptes créer par un employee
        Rest::Routes::Get(this->t_router, "/employee/account/get/:id",
                          Rest::Routes::bind(&RequestHandler::getAccountByEmployeeId, &this->t_rhandler)) ;
        // Tout les employées
        Rest::Routes::Get(this->t_router, "/employee/get/all",
                          Rest::Routes::bind(&RequestHandler::getAllEmployees, &this->t_rhandler)) ;
        // Un employee avec un id
        Rest::Routes::Get(this->t_router, "/employee/get/:id",
                          Rest::Routes::bind(&RequestHandler::getEmployeeById, &this->t_rhandler)) ;
        // Tout les clients
        Rest::Routes::Get(this->t_router, "/customer/get/all",
                          Rest::Routes::bind(&RequestHandler::getAllCustomers, &this->t_rhandler)) ;
        // La liste des subordonnés d'un employé
        Rest::Routes::Get(this->t_router, "/employee/subordinate/get/all/:token",
                          Rest::Routes::bind(&RequestHandler::getSubordinates, &this->t_rhandler)) ;
        // Un client avec un id
        Rest::Routes::Get(this->t_router, "/customer/get/id/:value",
                          Rest::Routes::bind(&RequestHandler::getCustomerById, &this->t_rhandler)) ;
        // Un client avec un nom
        Rest::Routes::Get(this->t_router, "/customer/get/name/:value",
                          Rest::Routes::bind(&RequestHandler::getCustomerByName, &this->t_rhandler)) ;

        /* les requetes PUT */
        Rest::Routes::Put(this->t_router, "/user/account/update/:token",
                        Rest::Routes::bind(&RequestHandler::updateUserAccount, &this->t_rhandler)) ;

        /* Les requetes POST */
        // Ajout d'un compte
        Rest::Routes::Post(this->t_router, "/account/add/",
                            Rest::Routes::bind(&RequestHandler::addAccount, &this->t_rhandler)) ;
        // Ajout d'une opération
        Rest::Routes::Post(this->t_router, "/operation/add/",
                            Rest::Routes::bind(&RequestHandler::addOperation, &this->t_rhandler)) ;
        // La route pour l'authentification
        Rest::Routes::Post(this->t_router, "/authentification",
                            Rest::Routes::bind(&RequestHandler::authentification, &this->t_rhandler)) ;
        Rest::Routes::Post(this->t_router, "/subscription/:token",
                            Rest::Routes::bind(&RequestHandler::subscription, &this->t_rhandler));
        /* Les requetes DELETE */
        // Un client
        Rest::Routes::Delete(this->t_router, "/person/delete/:id",
                             Rest::Routes::bind(&RequestHandler::deletePerson, &this->t_rhandler)) ;
        // Un compte
        Rest::Routes::Delete(this->t_router, "/account/delete/:id",
                             Rest::Routes::bind(&RequestHandler::deleteAccount, &this->t_rhandler)) ;
        // Une operation
        Rest::Routes::Delete(this->t_router, "/operation/delete/:id",
                             Rest::Routes::bind(&RequestHandler::deleteOperation, &this->t_rhandler)) ;
    }

    Address RestServer::parseJson(const std::string &json)
    {
        rapidjson::Document doc ;
        if(doc.Parse(json.c_str()).HasParseError())
        {
            LOG_FATAL << "Impossible de lancer le serveur REST: json parse error !\n" ;
            std::exit(EXIT_FAILURE) ;
        }
        if(!doc.HasMember("address") || !doc.HasMember("port"))
        {
            LOG_FATAL << "Impossible de lancer le serveur REST: configuration incorrect !\n" ;
            std::exit(EXIT_FAILURE) ;
        }
        rapidjson::Value &adr = doc["address"] ;
        rapidjson::Value &port = doc["port"] ;
        LOG_INFO << "Serveur REST sur " << adr.GetString() <<":"<<port.GetInt() << " !\n" ;
        return Address(adr.GetString(), port.GetInt()) ;
    }

    RestServer::RestServer(char *hostname, int port)
    {
      Address adr(hostname, port) ;
      this->p_endpoint = std::make_shared<Http::Endpoint>(adr) ;
      this->routing() ;
      LOG_INFO << "Serveur REST sur " << hostname <<":"<< port << " !\n" ;
    }

    RestServer::RestServer(const std::string &json)
    {
        Address adr ;
        if(!json.size())
        {
            std::string json2 ;
            std::ifstream config("../../resources/config/rest/json") ;
            if(config.fail())
            {
                LOG_FATAL << "Impossible de lancer le serveur REST: configuration introuvable !\n" ;
                std::exit(EXIT_FAILURE) ;
            }
            // je me position à la fin du fichier
            config.seekg(std::ios::end) ;
            long filesize = config.tellg()+ 1L ;
            json2.resize(filesize) ;
            config.seekg(std::ios::beg) ;
            config.read(&json2[0], filesize) ;
            config.close() ;
            adr = this->parseJson(json2) ;
        }
        else
        {
            adr = this->parseJson(json) ;
        }

        this->p_endpoint = std::make_shared<Http::Endpoint>(adr) ;
        this->routing() ;
    }

    void RestServer::start()
    {
        auto opts = Http::Endpoint::options()
                .threads(2)
                .flags(Tcp::Options::InstallSignalHandler);
        this->p_endpoint->init(opts);
        this->routing() ;
        this->p_endpoint->setHandler(this->t_router.handler()) ;
        this->p_endpoint->serve() ;
    }

    void RestServer::stop()
    {
        this->p_endpoint->shutdown();
    }
}
