#ifndef __FLASHDEBUG_H__
#define __FLASHDEBUG_H__

extern "C" {
#include <quadflashlib.h>
#include <xcore/port.h>
#include <xcore/clock.h>
}
#include <string>
#include <vector>


class SerialiseToFlash {
public:
    SerialiseToFlash() {
        flashport = {
            PORT_SQI_CS,
            PORT_SQI_SCLK,
            PORT_SQI_SIO,
            XS1_CLKBLK_1
        };
        
    }


    int connect() {
        return fl_connect(&flashport);
    }

    void firstItemCheck() {
        if (firstItem) {
            firstItem = false;
        }else{
            data.push_back(',');
        }
    }

    void writeKey(std::string key) {
        data.push_back('\"');
        data.insert(data.end(), key.begin(), key.end());
        data.push_back('\"');
        data.push_back(':');
    }

    void addInt(std::string key, int value) {
        firstItemCheck();
        writeKey(key);
        data.push_back('\"');
        data.push_back('i');
        const uint8_t* valbytes = reinterpret_cast<const uint8_t*>(&value);
        for(size_t i=0; i < sizeof(int); i++) {
            sprintf(buffer, "%.2x", valbytes[i]);
            data.push_back(buffer[0]);
            data.push_back(buffer[1]);
        }
        data.push_back('\"');
    }

    void addFloat(std::string key, float value) {
        firstItemCheck();
        writeKey(key);
        data.push_back('\"');
        data.push_back('f');
        const uint8_t* valbytes = reinterpret_cast<const uint8_t*>(&value);
        for(size_t i=0; i < sizeof(float); i++) {
            sprintf(buffer, "%.2x", valbytes[i]);
            data.push_back(buffer[0]);
            data.push_back(buffer[1]);
        }
        data.push_back('\"');
    }

    void writeFloatArray(std::vector<float> value) {
        data.push_back('[');

        for(size_t i=0; i < value.size(); i++) {
            data.push_back('\"');
            data.push_back('f');
            const uint8_t* valbytes = reinterpret_cast<const uint8_t*>(&value[i]);
            for(size_t i=0; i < sizeof(float); i++) {
                sprintf(buffer, "%.2x", valbytes[i]);
                data.push_back(buffer[0]);
                data.push_back(buffer[1]);
            }
            data.push_back('\"');
            if (i < value.size()-1) data.push_back(',');
        }
        data.push_back(']');

    }

    void addFloatVector2D(std::string key, std::vector< std::vector<float>> value) {
        firstItemCheck();
        writeKey(key);
        data.push_back('[');
        for(size_t i=0; i < value.size(); i++) {
            writeFloatArray(value.at(i));
            if (i < value.size()-1) data.push_back(',');
        }
        data.push_back(']');

    }

    void addFloatVector(std::string key, std::vector<float> value) {
        firstItemCheck();
        writeKey(key);
        writeFloatArray(value);
    }

    void writeToFlash() {
        data.push_back('}');
        //write in data size
        size_t datalen = data.size() - 16;
        const uint8_t* valbytes = reinterpret_cast<const uint8_t*>(&datalen);
        size_t idx=8;
        for(size_t i=0; i < sizeof(size_t); i++) {
            sprintf(buffer, "%.2x", valbytes[i]);
            data[idx++] = buffer[0];
            data[idx++] = buffer[1];
        }
        size_t scratchSize = fl_getWriteScratchSize(0, data.size());
        printf("# scratch size: %x\n", scratchSize);
        std::vector<unsigned char> scratch(scratchSize);
        size_t writeRes = fl_writeData(0, data.size(), data.data(), scratch.data());
        printf("write: %x\n", writeRes);

    }

    void printFlashInfo() {
        //get info
        size_t dataPartitionSize = fl_getDataPartitionSize();
        printf("Data partition size: %x\n", dataPartitionSize);
        size_t numDataSectors = fl_getNumDataSectors();
        printf("# data sectors: %x\n", numDataSectors);
        size_t pageSize = fl_getPageSize();
        printf("page size: %x\n", pageSize);
        size_t numDataPages = fl_getNumDataPages();
        printf("# data pages: %x\n", numDataPages);
        // size_t dataSectorSize = fl_getDataSectorSize();
        // printf("Data sector size: %x\n", dataSectorSize);
    }
private:
    std::vector<uint8_t> data = {'M','E','M','L','D','A','T','A',0,0,0,0,0,0,0,0,'{'};
    fl_QSPIPorts flashport;
    bool firstItem=true;
    char buffer[16];

};


static SerialiseToFlash gFlash_serialiser;

#endif 