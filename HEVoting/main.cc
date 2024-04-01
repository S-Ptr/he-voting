#include <drogon/drogon.h>
int main() {
    try{
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0",3000);
    //Load config file
    drogon::app().loadConfigFile("../../../config.json");
    //Run HTTP framework,the method will block in the internal event loop

    //CORS cringe code
    //Register pre-routing advice to handle OPTIONS requests
    drogon::app().registerPreRoutingAdvice([](const drogon::HttpRequestPtr& req,
        drogon::AdviceCallback&& cb,
        drogon::AdviceChainCallback&& ccb) {
            if (req->method() == drogon::HttpMethod::Options) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->addHeader("Access-Control-Allow-Origin", "http://localhost:4200");
                resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
                resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
                cb(resp);
            }
            else {
                //Call the next callback in the chain for non-OPTIONS requests
                ccb();
            }
        });

    //drogon::app().enableSession(1800);

    drogon::app().registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr& req, const drogon::HttpResponsePtr& resp) {
            //LOG_DEBUG << "postHandling1";
            resp->addHeader("Access-Control-Allow-Origin", "http://localhost:4200");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
        }
    );
    
    for (auto& listener : drogon::app().getCustomConfig()["listeners"]) {
        LOG_INFO << "Listening on port" << listener["port"].asString();
    }
    drogon::app().run();
    LOG_INFO << "Server shutting down. Hasta la vista!";
    return 0;
	}
	catch(std::invalid_argument& e){
        std::cout << "im in main lmao" << "\n";
		std::cerr << e.what() << std::endl;
	}
}
