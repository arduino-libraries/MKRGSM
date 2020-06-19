#pragma once
#include <Arduino.h>

String readLine()
{
    String line;

    while (1)
    {
        if (Serial.available())
        {
            char c = Serial.read();

            if (c == '\r')
            {
                // ignore
            }
            else if (c == '\n')
            {
                break;
            }

            line += c;
        }
    }

    line.trim();

    return line;
}

String promptAndReadLine(const char* prompt, const char* defaultValue) {
  Serial.print(prompt);
  Serial.print(" [");
  Serial.print(defaultValue);
  Serial.print("]: ");

  String s = readLine();

  if (s.length() == 0) {
    s = defaultValue;
  }

  Serial.println(s);

  return s;
}

int promptAndReadInt(const char* prompt, const int defaultValue) {
  Serial.print(prompt);
  Serial.print(" [");
  Serial.print(defaultValue);
  Serial.print("]: ");

  String s = readLine();
  int r;

  if (s.length() == 0) {
    r = defaultValue;
  } else {
      r = s.toInt();
  }

  Serial.println(r);

  return r;
}

String promptAndReadLine(const char *prompt)
{
    Serial.print(prompt);
    String s = readLine();
    Serial.println(s);

    return s;
}

int promptAndReadInt(const char *prompt)
{
    Serial.print(prompt);
    String s = readLine();
    Serial.println(s);

    return s.toInt();
}


String toHex(char c)
{
    String hex;

    hex = "0x";
    if (c < 16)
        hex += "0";
    hex += String(c, HEX);

    return hex;
}

void printHex(const String& buf, const unsigned int cols)
{
    for (size_t i = 0; i < buf.length(); i++) {
        String hex = toHex(buf[i]);
        hex += " ";
        Serial.print(hex);
        if (i % cols == (cols - 1))
            Serial.println();
    }
    Serial.println();
}

void printHex(const uint8_t* buf, const size_t len, const unsigned int cols)
{
    for (size_t i = 0; i < len; i++) {
        //    Serial.print("0x");
        if (buf[i] < 16)
            Serial.print(0);
        Serial.print(buf[i], HEX);
        //    if (i != len - 1) Serial.print(", ");
        if (i % cols == (cols - 1))
            Serial.println();
    }
    Serial.println();
}
