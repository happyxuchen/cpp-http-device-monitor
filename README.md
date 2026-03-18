# C++ HTTP Device Monitor API

A lightweight HTTP-based device monitoring system built in C++ using cpp-httplib.  
This project simulates an industrial telemetry processing service that ingests device data, validates inputs, detects abnormal conditions, and exposes monitoring APIs.

---

## 🚀 Overview

This system implements a REST-style HTTP server that accepts telemetry data from simulated devices and provides endpoints for monitoring device status and alerts.

The project focuses on:

- C++ server-side development
- HTTP API design
- Input validation and error handling
- Alert detection logic
- Logging and observability
- Testing and documentation

---

## 🏗️ Architecture

Client (curl / device)
↓
HTTP Server (cpp-httplib)
↓
Data Parsing & Validation
↓
Alert Detection Engine
↓
In-memory Storage
↓
Logging System


---

## ⚙️ Features

- HTTP server implemented in C++ (cpp-httplib)
- REST-style endpoints for device monitoring
- Structured telemetry ingestion (device, temperature, pressure, status)
- Input validation and error handling
- Threshold-based alert detection
- JSON response formatting
- Persistent logging to file
- Manual API testing with curl

---

## 📡 API Endpoints

```
1. Health Check

GET /health


Response:

json
{"status":"ok"}

2. Submit Device Data
POST /devices

Example:

curl -X POST "http://localhost:8082/devices?name=sensor1&temp=40&pressure=113&status=OK"

Response:

{"message":"device stored"}

Or (if alerts triggered):

{"message":"device stored with alerts"}

3. Get Devices
GET /devices

Response:

[
  {
    "name":"sensor1",
    "temp":40,
    "pressure":113,
    "status":"OK"
  }
]

4. Get Alerts
GET /alerts

Response:

[
  "High temperature detected for sensor1",
  "High pressure detected for sensor1"
]

```
---

## 🛡️ Validation & Error Handling

The system performs defensive validation on incoming requests:

Missing fields
curl -X POST "http://localhost:8082/devices?name=sensor1&temp=40&status=OK"

Response:

{"error":"missing required fields"}
Invalid numeric input
curl -X POST "http://localhost:8082/devices?name=sensor1&temp=abc&pressure=113&status=OK"

Response:

{"error":"invalid numeric value"}

---

## 🚨 Alert Logic

Alerts are triggered when:

Temperature > 35 → High temperature alert

Pressure > 110 → High pressure alert

---

## 📝 Logging

All incoming device data and alerts are logged to:

logs/http_device_monitor.log

Example:

[2026-03-18 12:30:00] DEVICE=sensor1, TEMP=40, PRESSURE=113, STATUS=OK
  ALERTS: High temperature detected for sensor1; High pressure detected for sensor1

---

## 🧪 Testing

Testing is performed using:

curl (manual API testing)

edge case validation

malformed input testing

See:

TEST_PROCEDURE.md

SPEC.md

---

## 🛠️ Build & Run

Compile
clang++ -std=c++17 src/server.cpp -o build/server
Run
./build/server

Server will start at:

http://localhost:8082

---

## 📂 Project Structure

cpp-http-device-monitor
├── include/
│   └── httplib.h
├── src/
│   └── server.cpp
├── logs/
├── build/
├── README.md
├── SPEC.md
├── TEST_PROCEDURE.md

---

## 🔮 Future Improvements

JSON request body support

Persistent database storage

Multi-threaded request handling

Integration with industrial protocols (Modbus, OPC-UA)

Web-based monitoring dashboard



