#include "util/logger.h"

#include <gtest/gtest.h>

std::string filename = "";

TEST(LoggerTest, Log) {
    Logger::GetInst().init(Logger::LogLevel::TRACE);

    filename = Logger::GetInst().getLogFileName();
    std::cout << filename << std::endl;

    for (int i = 1; i <= 3; ++i) {
        for (int j = 1; j <= 3; ++j) {
            LOG_TRACE("Trace log: i = {}, j = {}, i * j = {}.", i, j, i * j);
            LOG_DEBUG("Debug log: i = {}, j = {}, i * j = {}.", i, j, i * j);
            LOG_INFO("Info log: i = {}, j = {}, i * j = {}.", i, j, i * j);
            LOG_WARNING("Warning log: i = {}, j = {}, i * j = {}.", i, j, i * j);
            LOG_ERROR("Error log: i = {}, j = {}, i * j = {}.", i, j, i * j);
            LOG_FATAL("Fatal log: i = {}, j = {}, i * j = {}.", i, j, i * j);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // flush
    Logger::GetInst().flush();

    // count the number of lines in the log file
    int line_count = 0;
    char ch;
    FILE *fp = fopen(filename.c_str(), "r");
    EXPECT_NE(fp, nullptr);
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }
    fclose(fp);
    EXPECT_EQ(line_count, 54 * 2);  // 6 log levels * 9 iterations

    // erase the log file
    std::remove(filename.c_str());
}