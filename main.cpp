#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <mutex>
std::mutex userFileMutex;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace web::json;
const std::wstring filePath = L"users.json";
unsigned long simpleHash(const std::wstring& str) {
    unsigned long hash = 5381;
    for (wchar_t c : str) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    return hash;
}

// Helper function to convert hash to wstring
std::wstring hashToWString(unsigned long hash) {
    std::wstringstream ss;
    ss << hash;
    return ss.str();
}
json::value loadUsersFromFile() {
    json::value users = json::value::object();
    if (std::filesystem::exists(filePath)) {
        std::ifstream inFile(filePath);
        if (inFile) {
            std::stringstream buffer;
            buffer << inFile.rdbuf();
            inFile.close();
            try {
                users = json::value::parse(utility::conversions::to_string_t(buffer.str()));
            }
            catch (const std::exception& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }
    }
    return users;
}

// Save users to the file
void saveUsersToFile(const json::value& users) {
    std::ofstream outFile(filePath);
    if (outFile) {
        outFile << utility::conversions::to_utf8string(users.serialize());
        outFile.close();
    }
    else {
        std::cerr << "Failed to write to user file" << std::endl;
    }
}

void registerUser(json::object& jsonObject, http_request request) {
    json::value users = loadUsersFromFile();

    std::wstring wUsername = jsonObject[U("username")].as_string();
    std::wstring wPassword = jsonObject[U("password")].as_string();
    {
        std::lock_guard<std::mutex> lock(userFileMutex);
        if (users.has_field(wUsername)) {
            json::value response;
            response[U("error")] = json::value::string(U("Username already exists"));
            request.reply(web::http::status_codes::Conflict, response);  // 409 Conflict
            return;
        }
        users[wUsername] = json::value::string(hashToWString(simpleHash(wPassword)));
        saveUsersToFile(users);
    }

    // Send success response
    json::value response_data;
    response_data[U("message")] = json::value::string(U("User registered successfully"));
    request.reply(web::http::status_codes::OK, response_data);
}
void loginUser(json::object& jsonObject, http_request request) {
    json::value users = loadUsersFromFile();
    std::wstring wUsername = jsonObject[U("username")].as_string();
    std::wstring wPassword = jsonObject[U("password")].as_string();
    {
        std::lock_guard<std::mutex> lock(userFileMutex);

        // Check if the user already exists
        if (users.has_field(wUsername)) {
            std::wcout << L"found user " << wUsername << L" trying to log in" << std::endl;
            std::wstring passFormat = U("\"") + hashToWString(simpleHash(wPassword)) + U("\"");
            if (users[wUsername].to_string() == passFormat) {
                json::value response;
                response[U("success")] = json::value::string(U("User logged in"));
                request.reply(web::http::status_codes::OK, response);  // 200 OK
                return;
            }
            else {
                json::value response;
                response[U("error")] = json::value::string(U("Wrong password"));
                request.reply(web::http::status_codes::Conflict, response);  // 409 Conflict
                return;
            }
        }
        else {
            json::value response;
            response[U("error")] = json::value::string(U("No user found! Maybe you are trying to register?"));
            request.reply(web::http::status_codes::Conflict, response);  // 409 Conflict
        }

    }

}
void handleGet(http_request request) {
	json::value response_data;
	response_data[U("message")] = json::value::string(U("Hello :)"));
	request.reply(status_codes::OK, response_data);
}
void handlePost(http_request request) {
    request.extract_json().then([=](pplx::task<web::json::value> task) {
        try {
            auto json_body = task.get();  // Get the JSON body
            std::wcout << L"Received raw body: " << utility::conversions::to_utf16string(json_body.serialize()) << std::endl;

            if (json_body.has_field(U("reg"))) {
                auto regData = json_body[U("reg")].as_object();
                auto username = regData[U("username")].as_string();
                auto password = regData[U("password")].as_string();

                std::wcout << L"Registering user " << username << L" " << password << std::endl;
                registerUser(regData, request);  // Call your register function
                std::cout << "User registration processed." << std::endl;
            }
            else if (json_body.has_field(U("log"))) {
                auto logData = json_body[U("log")].as_object();
                auto username = logData[U("username")].as_string();
                auto password = logData[U("password")].as_string();
                std::wcout << L"trying to log in " << username << std::endl;
                loginUser(logData, request);
            }
            else {
                std::cerr << "Error: Missing 'reg' field in the request body." << std::endl;
                json::value response_data;
                response_data[U("error")] = json::value::string(U("Missing 'reg' or 'log' field"));
                request.reply(status_codes::BadRequest, response_data);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            json::value response_data;
            response_data[U("error")] = json::value::string(U("JSON parsing error"));
            request.reply(status_codes::BadRequest, response_data);
        }
        }).wait();
}
int main() {
	uri_builder uri(U("http://localhost:8080"));
	http_listener listener(uri.to_uri());
	listener.support(methods::GET, handleGet);
	listener.support(methods::POST, handlePost);
	try {
		listener.open().then([&listener]() { std::wcout << "Listening on " << listener.uri().to_string() << std::endl;}).wait();
		std::string line;
		std::getline(std::cin, line);
	}
	catch (const std::exception& e) {
		std::cerr << "Erorr " << e.what() << std::endl;
	}
	return 0;
}