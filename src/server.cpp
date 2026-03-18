#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <ctime>
#include "../include/httplib.h"

struct Device
{
    std::string name;
    int temp;
    int pressure;
    std::string status;
};

std::vector<Device> devices;
std::vector<std::string> alerts;

std::string escape_json(const std::string &s)
{
    std::string out;
    for (char c : s)
    {
        if (c == '\"')
            out += "\\\"";
        else
            out += c;
    }
    return out;
}

std::string get_current_time()
{
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

void write_log(const Device &d, const std::vector<std::string> &new_alerts)
{
    std::ofstream log_file("logs/http_device_monitor.log", std::ios::app);
    if (!log_file.is_open())
        return;

    log_file << "[" << get_current_time() << "] "
             << "DEVICE=" << d.name
             << ", TEMP=" << d.temp
             << ", PRESSURE=" << d.pressure
             << ", STATUS=" << d.status << "\n";

    if (!new_alerts.empty())
    {
        log_file << "  ALERTS: ";
        for (size_t i = 0; i < new_alerts.size(); i++)
        {
            log_file << new_alerts[i];
            if (i + 1 < new_alerts.size())
                log_file << "; ";
        }
        log_file << "\n";
    }

    log_file << "\n";
}

std::string devices_to_json()
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < devices.size(); i++)
    {
        const auto &d = devices[i];
        oss << "{"
            << "\"name\":\"" << escape_json(d.name) << "\","
            << "\"temp\":" << d.temp << ","
            << "\"pressure\":" << d.pressure << ","
            << "\"status\":\"" << escape_json(d.status) << "\""
            << "}";
        if (i + 1 < devices.size())
            oss << ",";
    }
    oss << "]";
    return oss.str();
}

std::string alerts_to_json()
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < alerts.size(); i++)
    {
        oss << "\"" << escape_json(alerts[i]) << "\"";
        if (i + 1 < alerts.size())
            oss << ",";
    }
    oss << "]";
    return oss.str();
}

int main()
{
    httplib::Server svr;

    svr.Get("/health", [](const httplib::Request &, httplib::Response &res)
            { res.set_content("{\"status\":\"ok\"}", "application/json"); });

    svr.Get("/devices", [](const httplib::Request &, httplib::Response &res)
            { res.set_content(devices_to_json(), "application/json"); });

    svr.Get("/alerts", [](const httplib::Request &, httplib::Response &res)
            { res.set_content(alerts_to_json(), "application/json"); });

    svr.Post("/devices", [](const httplib::Request &req, httplib::Response &res)
             {
        auto name = req.get_param_value("name");
        auto temp_str = req.get_param_value("temp");
        auto pressure_str = req.get_param_value("pressure");
        auto status = req.get_param_value("status");

        if (name.empty() || temp_str.empty() || pressure_str.empty() || status.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"missing required fields\"}", "application/json");
            return;
        }

        try {
            int temp = std::stoi(temp_str);
            int pressure = std::stoi(pressure_str);

            Device d{name, temp, pressure, status};
            devices.push_back(d);

            std::vector<std::string> new_alerts;

            if (temp > 35) {
                std::string msg = "High temperature detected for " + name;
                alerts.push_back(msg);
                new_alerts.push_back(msg);
            }

            if (pressure > 110) {
                std::string msg = "High pressure detected for " + name;
                alerts.push_back(msg);
                new_alerts.push_back(msg);
            }

            write_log(d, new_alerts);

            if (new_alerts.empty()) {
                res.set_content("{\"message\":\"device stored\"}", "application/json");
            } else {
                res.set_content("{\"message\":\"device stored with alerts\"}", "application/json");
            }

        } catch (...) {
            res.status = 400;
            res.set_content("{\"error\":\"invalid numeric value\"}", "application/json");
        } });

    std::cout << "HTTP server listening on http://localhost:8082\n";
    svr.listen("0.0.0.0", 8082);
    return 0;
}