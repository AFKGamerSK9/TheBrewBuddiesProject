#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "httplib.h"

std::string readRawFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    httplib::Server svr;

    // Serve static files (images, css, js) from the ./public directory directly
    svr.set_mount_point("/", "./public");

    // 1. Web Page Routes
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = readRawFile("public/index.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found - Index</h1>", "text/html"); }
    });

    svr.Get("/product", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = readRawFile("public/product.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found - Product</h1>", "text/html"); }
    });

    svr.Get("/account", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = readRawFile("public/account.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found - Account</h1>", "text/html"); }
    });

    svr.Get("/cart", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = readRawFile("public/cart.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<h1>404 Not Found - Cart</h1>", "text/html"); }
    });

    // 2. Component Routes
    svr.Get("/review-modal.html", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = readRawFile("public/review-modal.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<!-- Review modal missing -->", "text/html"); }
    });

    svr.Get("/profile-modal.html", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = readRawFile("public/profile-modal.html");
        if (!html.empty()) res.set_content(html, "text/html");
        else { res.status = 404; res.set_content("<!-- Profile modal missing -->", "text/html"); }
    });

    // 3. API Endpoints
    svr.Get("/api/products", [](const httplib::Request& req, httplib::Response& res) {
        std::string json = R"([
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

    svr.Post("/api/reviews", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[REVIEW RECEIVED] " << req.body << std::endl;
        res.set_content("{\"status\":\"success\",\"message\":\"Review submitted successfully\"}", "application/json");
    });

    // Dynamic Port Binding (binds to Render's environment PORT or defaults to 8080 locally)
    const char* port_env = std::getenv("PORT");
    int port = port_env ? std::stoi(port_env) : 8080;

    std::cout << "The Brew Buddies server live at port: " << port << std::endl;
    svr.listen("0.0.0.0", port);

    return 0;
}