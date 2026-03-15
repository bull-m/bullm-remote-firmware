#include "model/i2c.h"
#include "main.h"

size_t maxBufferSize = I2C_BUFFER_LENGTH;

std::map<uint8_t, uint8_t> i2cs;

void scan() {
    i2cs.clear();
    for (byte address = 0; address <= 127; address++) {
        Wire.beginTransmission(address);
        i2cs[address] = Wire.endTransmission();
    }
}

bool I2cScan(byte address) {
    Wire.beginTransmission(address);
    return Wire.endTransmission() == 0;
}

JsonDocument *I2cHandle(JsonDocument &data) {
    String mode = data["mode"];
    auto *req = new JsonDocument();
    auto &json = *req;
    JsonArray arr = json["data"].to<JsonArray>();
    if (mode == "scan") {
        scan();
        for (const auto &entry: i2cs) {
            arr.add(entry.second);
        }
    }
    return req;
}

void I2cInit() {
    Wire.begin(PIN_SDA, PIN_SCL, 100000UL); // ,100000

//    // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
//    // some i2c devices dont like this so much so if you're sharing the bus, watch
//    // out for this!
//    Wire.setClock(400000);
}

bool I2cWrite(uint8_t address, const uint8_t *buffer, size_t len, bool stop) {
    if (len > maxBufferSize) {
        return false;
    }
    Wire.beginTransmission(address);
    // 写入数据
    if (Wire.write(buffer, len) != len) {
        return false; // 写入失败
    }
    return Wire.endTransmission(stop) == 0;
}


bool _read(uint8_t address, uint8_t *buffer, size_t len, bool stop) {
    size_t recv = Wire.requestFrom((uint8_t) address, (uint8_t) len, (uint8_t) stop);
    // 没有足够的数据
    if (recv != len) return false;

    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = Wire.read();
    }
    return true;
}

bool I2cRead(uint8_t address, uint8_t *buffer, size_t len, bool stop) {
    size_t pos = 0;
    while (pos < len) {
        // 要读取的字节数
        size_t read_len = ((len - pos) > maxBufferSize) ? maxBufferSize : (len - pos);
        bool read_stop = (pos >= len - read_len) && stop;
        if (!_read(address, buffer + pos, read_len, read_stop))
            return false;
        pos += read_len;
    }
    return true;
}

/**
 * 写入并读取一些数据
 * @param  write_buffer 要写入的数据缓冲区的数据
 * @param  write_len 要写入的长度
 * @param  read_buffer 要读取的数据缓冲区
 * @param  read_len 要读取的字节数
 * @param  stop 读取时是否发送I2C STOP信号
 * @return 是否成功
 */
bool I2cWriteThenRead(uint8_t address, const uint8_t *write_buffer,
                      size_t write_len, uint8_t *read_buffer,
                      size_t read_len, bool stop) {
    if (!I2cWrite(address, write_buffer, write_len, stop)) {
        return false;
    }

    return I2cRead(address, read_buffer, read_len);
}