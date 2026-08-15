#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>
#include "httplib.h"
using namespace std;

// ============================================================
// READ A FILE
// ============================================================
string readRawFile(const string& path)
{
    ifstream file(path);
    if (!file.is_open())
    {
        cerr << "Failed to open file: " << path << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ============================================================
// TRIM WHITESPACE
// ============================================================
string trim(const string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// ============================================================
// ROBUST CSV LINE PARSER (Handles quotes and commas correctly)
// ============================================================
vector<string> parseCsvLine(const string& line)
{
    vector<string> result;
    string current = "";
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '"')
        {
            inQuotes = !inQuotes;
        }
        else if (c == ',' && !inQuotes)
        {
            result.push_back(current);
            current = "";
        }
        else
        {
            current += c;
        }
    }
    result.push_back(current);
    return result;
}

// ============================================================
// EXTRACT FIELD VALUE FROM JSON PAYLOAD
// ============================================================
string getJsonField(const string& json, const string& fieldName)
{
    const string key = "\"" + fieldName + "\"";
    size_t keyPos = json.find(key);
    if (keyPos == string::npos)
        return "";
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == string::npos)
        return "";
    size_t valStart = colonPos + 1;
    while (valStart < json.size() && isspace(static_cast<unsigned char>(json[valStart])))
    {
        valStart++;
    }
    if (valStart >= json.size())
        return "";

    if (json[valStart] == '"')
    {
        // String value
        valStart++;
        size_t valEnd = json.find('"', valStart);
        if (valEnd == string::npos)
            return "";
        return json.substr(valStart, valEnd - valStart);
    }
    else
    {
        // Number or boolean value
        size_t valEnd = valStart;
        while (valEnd < json.size() && json[valEnd] != ',' && json[valEnd] != '}' && !isspace(static_cast<unsigned char>(json[valEnd])))
        {
            valEnd++;
        }
        return json.substr(valStart, valEnd - valStart);
    }
}

// ============================================================
// MAIN
// ============================================================
int main()
{
    httplib::Server svr;

    // ========================================================
    // SERVE PUBLIC FOLDER & DATA
    // ========================================================
    svr.set_mount_point("/", "./public");
    svr.set_mount_point("/data", "./data");

    // ========================================================
    // WEB PAGE ROUTES
    // ========================================================
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/index.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found</h1>", "text/html"); }
    });
    svr.Get("/product", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/product.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found</h1>", "text/html"); }
    });
    svr.Get("/account", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/account.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found</h1>", "text/html"); }
    });
    svr.Get("/cart", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/cart.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found</h1>", "text/html"); }
    });
    svr.Get("/about", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/about.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found</h1>", "text/html"); }
    });
    svr.Get("/settings", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/settings.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found</h1>", "text/html"); }
    });
    svr.Get("/support", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/support.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found</h1>", "text/html"); }
    });

    // ========================================================
    // COMPONENT ROUTES
    // ========================================================
    svr.Get("/cart-drawer.html", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/cart-drawer.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else res.status = 404;
    });
    svr.Get("/review-modal.html", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/review-modal.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else res.status = 404;
    });
    svr.Get("/profile-modal.html", [](const httplib::Request& req, httplib::Response& res)
    {
        string html = readRawFile("public/profile-modal.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else res.status = 404;
    });

    // ========================================================
    // PRODUCTS API
    // ========================================================
    svr.Get("/api/products", [](const httplib::Request& req, httplib::Response& res)
    {
        string json = R"([
            {
                "id": 1,
                "name": "Ethiopian Yirgacheffe G1",
                "tag": "SINGLE ORIGIN",
                "rating": 4.9,
                "reviewCount": 128,
                "price": 1250,
                "image": "https://images.unsplash.com/photo-1559056199-641a0ac8b55e?auto=format&fit=crop&w=600&q=80"
            },
            {
                "id": 2,
                "name": "Guatemala Antigua Medium",
                "tag": "BEST SELLER",
                "rating": 4.8,
                "reviewCount": 94,
                "price": 1100,
                "image": "https://images.unsplash.com/photo-1587734195503-904fca47e0e9?auto=format&fit=crop&w=600&q=80"
            },
            {
                "id": 3,
                "name": "Sumatra Mandheling Dark",
                "tag": "DARK ROAST",
                "rating": 4.7,
                "reviewCount": 82,
                "price": 1350,
                "image": "https://images.unsplash.com/photo-1611854779393-1b2da9d400fe?auto=format&fit=crop&w=600&q=80"
            }
        ])";
        res.set_content(json, "application/json");
    });

    // ========================================================
    // GET USERS API (Returns users.csv records as JSON array)
    // ========================================================
    svr.Get("/api/users", [](const httplib::Request& req, httplib::Response& res)
    {
        const string filepath = "data/users.csv";
        ifstream file(filepath);
        if (!file.is_open())
        {
            res.set_content("[]", "application/json");
            return;
        }

        string line;
        getline(file, line); // Skip header

        string jsonArray = "[";
        bool first = true;

        while (getline(file, line))
        {
            if (line.empty()) continue;
            vector<string> fields = parseCsvLine(line);
            if (fields.size() < 10) continue;

            string id = fields[0];
            string name = fields[1];
            string email = fields[2];
            string phone = fields[3];
            string password = fields[4];
            string initials = fields[5];
            string tier = fields[6];
            string points = fields[7];
            string pointsVal = fields[8];
            string sub = fields[9];

            auto unquote = [](string s) {
                if (!s.empty() && s.front() == '"' && s.back() == '"')
                    return s.substr(1, s.length() - 2);
                return s;
            };

            name = unquote(name);
            email = unquote(email);
            phone = unquote(phone);
            password = unquote(password);
            initials = unquote(initials);
            tier = unquote(tier);
            pointsVal = unquote(pointsVal);
            sub = unquote(sub);

            if (!first) jsonArray += ",";
            first = false;

            jsonArray += "{";
            jsonArray += "\"id\":" + (id.empty() ? "1" : id) + ",";
            jsonArray += "\"name\":\"" + name + "\",";
            jsonArray += "\"email\":\"" + email + "\",";
            jsonArray += "\"password\":\"" + password + "\",";
            jsonArray += "\"phone\":\"" + phone + "\",";
            jsonArray += "\"avatarInitials\":\"" + initials + "\",";
            jsonArray += "\"membershipTier\":\"" + tier + "\",";
            jsonArray += "\"points\":" + (points.empty() ? "50" : points) + ",";
            jsonArray += "\"pointsValue\":\"" + pointsVal + "\",";
            jsonArray += "\"subscription\":\"" + sub + "\"";
            jsonArray += "}";
        }
        jsonArray += "]";
        res.set_content(jsonArray, "application/json");
    });

    // ========================================================
    // GET REVIEWS (CSV)
    // ========================================================
    svr.Get("/api/reviews", [](const httplib::Request& req, httplib::Response& res)
    {
        const string filepath = "data/reviews.csv";
        string content = readRawFile(filepath);

        if (content.empty())
        {
            content = "productId,rating,title,text,recommend,name,email,verified,date\n";
        }

        res.set_content(content, "text/csv");
    });

    // ========================================================
    // POST REVIEW (CSV APPEND)
    // ========================================================
    svr.Post("/api/reviews", [](const httplib::Request& req, httplib::Response& res)
    {
        const string filepath = "data/reviews.csv";
        if (req.body.empty())
        {
            res.status = 400;
            res.set_content(R"({"status":"error","message":"Empty request body"})", "application/json");
            return;
        }
        string productId = getJsonField(req.body, "productId");
        string rating = getJsonField(req.body, "rating");
        string title = getJsonField(req.body, "title");
        string text = getJsonField(req.body, "text");
        string recommend = getJsonField(req.body, "recommend");
        string name = getJsonField(req.body, "name");
        string email = getJsonField(req.body, "email");
        string verified = getJsonField(req.body, "verified");
        string date = getJsonField(req.body, "date");
        if (productId.empty())
        {
            res.status = 400;
            res.set_content(R"({"status":"error","message":"Invalid productId"})", "application/json");
            return;
        }
        stringstream csvLine;
        csvLine << productId << ","
                << (rating.empty() ? "5" : rating) << ","
                << "\"" << title << "\","
                << "\"" << text << "\","
                << (recommend.empty() ? "yes" : recommend) << ","
                << "\"" << name << "\","
                << (email.empty() ? "example@email.com" : email) << ","
                << (verified.empty() ? "True" : verified) << ","
                << "\"" << date << "\"\n";
        ofstream outFile(filepath, ios::out | ios::app);
        if (!outFile.is_open())
        {
            res.status = 500;
            res.set_content(R"({"status":"error","message":"Cannot open reviews.csv for writing"})", "application/json");
            return;
        }
        outFile << csvLine.str();
        outFile.close();
        res.status = 200;
        res.set_content(R"({"status":"success","message":"Review saved successfully to CSV"})", "application/json");
    });

    // ========================================================
    // POST USER REGISTRATION (USERS.CSV APPEND)
    // ========================================================
    svr.Post("/api/users", [](const httplib::Request& req, httplib::Response& res)
    {
        const string filepath = "data/users.csv";
        if (req.body.empty())
        {
            res.status = 400;
            res.set_content(R"({"status":"error","message":"Empty request body"})", "application/json");
            return;
        }

        string id = getJsonField(req.body, "id");
        string name = getJsonField(req.body, "name");
        string email = getJsonField(req.body, "email");
        string phone = getJsonField(req.body, "phone");
        string password = getJsonField(req.body, "password");
        string avatarInitials = getJsonField(req.body, "avatarInitials");
        string membershipTier = getJsonField(req.body, "membershipTier");
        string points = getJsonField(req.body, "points");
        string pointsValue = getJsonField(req.body, "pointsValue");
        string subscription = getJsonField(req.body, "subscription");

        if (email.empty() || password.empty())
        {
            res.status = 400;
            res.set_content(R"({"status":"error","message":"Invalid email or password"})", "application/json");
            return;
        }

        ifstream checkFile(filepath);
        bool isEmpty = !checkFile.is_open() || checkFile.peek() == ifstream::traits_type::eof();
        checkFile.close();

        ofstream outFile(filepath, ios::out | ios::app);
        if (!outFile.is_open())
        {
            res.status = 500;
            res.set_content(R"({"status":"error","message":"Cannot open users.csv for writing"})", "application/json");
            return;
        }

        if (isEmpty)
        {
            outFile << "id,name,email,phone,password,avatarInitials,membershipTier,points,pointsValue,subscription\n";
        }

        stringstream csvLine;
        csvLine << (id.empty() ? "2" : id) << ","
                << "\"" << name << "\","
                << "\"" << email << "\","
                << "\"" << phone << "\","
                << "\"" << password << "\","
                << avatarInitials << ","
                << "\"" << membershipTier << "\","
                << (points.empty() ? "50" : points) << ","
                << "\"" << (pointsValue.empty() ? "NRs. 50" : pointsValue) << "\","
                << "\"" << (subscription.empty() ? "No active subscription" : subscription) << "\"\n";

        outFile << csvLine.str();
        outFile.close();

        res.status = 200;
        res.set_content(R"({"status":"success","message":"User saved successfully"})", "application/json");
    });

    // ========================================================
    // POST USER LOGIN / AUTHENTICATION API
    // ========================================================
    svr.Post("/api/login", [](const httplib::Request& req, httplib::Response& res)
    {
        const string filepath = "data/users.csv";
        string emailInput = getJsonField(req.body, "email");
        string passInput = getJsonField(req.body, "password");

        ifstream file(filepath);
        if (!file.is_open())
        {
            res.status = 500;
            res.set_content(R"({"status":"error","message":"Database error"})", "application/json");
            return;
        }

        string line;
        getline(file, line); // Skip header

        bool authenticated = false;
        string foundId = "1", foundName = "", foundEmail = "", foundPhone = "", foundPass = "", foundInitials = "U", foundTier = "", foundPoints = "50", foundVal = "NRs. 50", foundSub = "";

        while (getline(file, line))
        {
            if (line.empty()) continue;
            vector<string> fields = parseCsvLine(line);
            if (fields.size() < 10) continue;

            string id = fields[0];
            string name = fields[1];
            string email = fields[2];
            string phone = fields[3];
            string password = fields[4];
            string initials = fields[5];
            string tier = fields[6];
            string points = fields[7];
            string pointsVal = fields[8];
            string sub = fields[9];

            auto unquote = [](string s) {
                if (!s.empty() && s.front() == '"' && s.back() == '"')
                    return s.substr(1, s.length() - 2);
                return s;
            };

            string cleanEmail = unquote(email);
            string cleanPass = unquote(password);

            if (cleanEmail == emailInput && cleanPass == passInput)
            {
                authenticated = true;
                foundId = id.empty() ? "1" : id;
                foundName = unquote(name);
                foundEmail = cleanEmail;
                foundPhone = unquote(phone);
                foundPass = cleanPass;
                foundInitials = unquote(initials);
                foundTier = unquote(tier);
                foundPoints = points.empty() ? "50" : points;
                foundVal = unquote(pointsVal);
                foundSub = unquote(sub);
                break;
            }
        }
        file.close();

        if (authenticated)
        {
            string jsonResp = "{";
            jsonResp += "\"status\":\"success\",";
            jsonResp += "\"message\":\"Authenticated\",";
            jsonResp += "\"user\":{";
            jsonResp += "\"id\":" + foundId + ",";
            jsonResp += "\"name\":\"" + foundName + "\",";
            jsonResp += "\"email\":\"" + foundEmail + "\",";
            jsonResp += "\"password\":\"" + foundPass + "\",";
            jsonResp += "\"phone\":\"" + foundPhone + "\",";
            jsonResp += "\"avatarInitials\":\"" + foundInitials + "\",";
            jsonResp += "\"membershipTier\":\"" + foundTier + "\",";
            jsonResp += "\"points\":" + foundPoints + ",";
            jsonResp += "\"pointsValue\":\"" + foundVal + "\",";
            jsonResp += "\"subscription\":\"" + foundSub + "\"";
            jsonResp += "}}";

            res.status = 200;
            res.set_content(jsonResp, "application/json");
        }
        else
        {
            res.status = 401;
            res.set_content(R"({"status":"error","message":"Invalid email or password"})", "application/json");
        }
    });

    // ========================================================
    // SERVER START
    // ========================================================
    const char* portEnv = getenv("PORT");
    int port = portEnv ? stoi(portEnv) : 8080;
    cout << endl;
    cout << "========================================" << endl;
    cout << "       THE BREW BUDDIES SERVER" << endl;
    cout << "========================================" << endl;
    cout << "Server running on port: " << port << endl;
    cout << "Open: http://localhost:" << port << endl;
    cout << "========================================" << endl;
    cout << endl;
    if (!svr.listen("0.0.0.0", port))
    {
        cerr << "[ERROR] Failed to start server" << endl;
        return 1;
    }
    return 0;
}