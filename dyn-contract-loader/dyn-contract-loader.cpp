// dyn-contract-loader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "nlohmann/json.hpp"
#include <curl\curl.h>


struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}


int main()
{
    using json = nlohmann::json;

    json j = "{ \"id\": 0, \"method\" : \"sendtoaddress\", \"params\" : [ \"dy1qxj4awv48k7nelvwwserdl9wha2mfg6w3wy05fc\" , 0.1], \"contract_code\" : \"01020304\"  }"_json;

    std::string jData = j.dump();

    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);
    chunk.size = 0;

    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    std::string transactionHex;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.62:6433");
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERNAME, "user");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "123456");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jData.c_str());

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else {
            json result = json::parse(chunk.memory);
            printf("%s\n", result.dump().c_str());
            transactionHex = result["result"];
        }

    }

    curl_easy_cleanup(curl);

    free(chunk.memory);

    curl_global_cleanup();


    /*

    json j = "{ \"id\": 0, \"method\" : \"createrawtransaction\", \"params\" : [ [], [{\"dy1qxj4awv48k7nelvwwserdl9wha2mfg6w3wy05fc\" : 0.1}] ] }"_json;

    std::string jData = j.dump();

    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);
    chunk.size = 0;

    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    std::string transactionHex;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.62:6433");
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERNAME, "user");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "123456");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jData.c_str());

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else {
            json result = json::parse(chunk.memory);
            printf("%s\n", result.dump().c_str());
            transactionHex = result["result"];
        }

    }

    std::string strRequest = "{ \"id\": 0, \"method\" : \"fundrawtransaction\", \"params\" : [\"" + transactionHex + "\"] }";

    j = json::parse(strRequest);

    jData = j.dump();
    chunk.size = 0;
    jData = j.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jData.c_str());

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    else {
        json result = json::parse(chunk.memory);
        printf("%s\n", result.dump().c_str());

    }


    curl_easy_cleanup(curl);

    free(chunk.memory);

    curl_global_cleanup();

    */
}

