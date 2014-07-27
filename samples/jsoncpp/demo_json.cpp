/*
 * demo_json.cpp
 * Created on: Jul 26, 2014
 * Author: chenguangyu
 */

#include <json/json.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int ParseJsonFromString() {
    const char* str =
            "{\"upload_id\": \"UP000000\",\"code\": 100,\"msg\": \"\",\"files\": \"\"}";
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(str, root)) {
        string upload_id = root["upload_id"].asString();
        cout << "upload_id: " << upload_id << endl;
        int code = root["code"].asInt();
        cout << "code: " << code << endl;
    }
    return 0;
}

Json::Value ParseJsonFromFile(const char* filename) {
    Json::Reader reader;
    Json::Value root;
    std::ifstream is;
    is.open(filename, std::ios::binary);
    if (reader.parse(is, root)) {
        std::string code;
        if (!root["files"].isNull()) {
            code = root["uploadid"].asString();
            cout << "code:" << code << endl;
        }
        code = root.get("uploadid", "null").asString();
        cout << "code:" << code << endl;
        int file_size = root["files"].size();
        cout << "file_size: " << file_size << endl;
        for (int i = 0; i < file_size; ++i) {
            Json::Value val_image = root["files"][i]["images"];
            int image_size = val_image.size();
            for (int j = 0; j < image_size; ++j) {
                std::string type = val_image[j]["type"].asString();
                std::string url = val_image[j]["url"].asString();
                cout << "type: " << type << "url: " << url << endl;
            }
        }
    }
    is.close();
    return root;
}

Json::Value AddToJson(Json::Value& root) {
    Json::Value arrayObj;
    Json::Value new_item0, new_item1;
    new_item0["date"] = "2014-05-20";
    new_item1["time"] = "13:14:00";
    arrayObj.append(new_item0);
    arrayObj.append(new_item1);
    int file_size = root["files"].size();
    for (int i = 0; i < file_size; ++i)
        root["files"][i]["exifs"] = arrayObj;
    return root;
}

void OutputJson(Json::Value& root) {
    std::string out1 = root.toStyledString();
    cout << "root.toStyledString()" << endl << out1;
    Json::FastWriter writer;
    std::string out2 = writer.write(root);
    cout << "writer.write(root)" << endl << out2;
}

int main(int argc, char **argv) {
    ParseJsonFromString();
    Json::Value root = ParseJsonFromFile("demo.json");
    root = AddToJson(root);
    OutputJson(root);
    return 0;
}
