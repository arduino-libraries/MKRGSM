#pragma once

#include <Arduino_JSON.h>


// Make the struct Printable to allow Serial.print-ing
struct Config : public Printable {
    String deviceId;
    int timestamp;

    String toJson()
    {
        JSONVar conf;

        conf["deviceId"] = deviceId;
        conf["timestamp"] = timestamp;

        return JSON.stringify(conf);
    }

    bool fromJSON(const String buf)
    {
        auto conf = JSON.parse(buf);

        if (!checkProperty(conf, "deviceId", "string"))
            return false;

        if (!checkProperty(conf, "timestamp", "number"))
            return false;

        deviceId = conf["deviceId"];
        timestamp = int { conf["timestamp"] };

        return true;
    }

    virtual size_t printTo(Print& p) const
    {
        size_t written { 0 };

        written += p.print("DeviceId:         ");
        written += p.println(deviceId);
        written += p.print("Timestamp:        ");
        written += p.println(timestamp);

        return written;
    }

private:
    bool checkProperty(JSONVar json, String property, String type)
    {
        if (!json.hasOwnProperty(property)) {
            Serial.print(property);
            Serial.println(" not found");
            return false;
        }

        if (JSON.typeof(json[property]) != type) {
            Serial.print(property);
            Serial.print(" != \"");
            Serial.print(type);
            Serial.println("\"");
            return false;
        }

        return true;
    }
};