#include "cpr/cpr.h"
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <curl/curl.h>
#include <gtest/gtest.h>
#include <string>


TEST(VersionTests, StringVersionExists) {
    EXPECT_FALSE(CPR_VERSION.empty());
}

TEST(VersionTests, StringVersionValid) {
    const std::string version{CPR_VERSION};
    ASSERT_FALSE(version.empty());

    // Check if the version string is: '\d+\.\d+\.\d+'
    bool digit = true;
    size_t dotCount = 0;
    for (size_t i = 0; i < version.size(); i++) {
        if (i == 0) {
            EXPECT_TRUE(std::isdigit(version[i]));
        } else if (digit) {
            if (version[i] == '.') {
                digit = false;
                dotCount++;
                continue;
            }
        }
        EXPECT_TRUE(std::isdigit(version[i]));
        digit = true;
    }
    EXPECT_EQ(dotCount, 2);
}

TEST(VersionTests, VersionPartsMatchString) {
    const std::string expected = std::to_string(CPR_VERSION_MAJOR) + "." + std::to_string(CPR_VERSION_MINOR) + "." + std::to_string(CPR_VERSION_PATCH);
    EXPECT_EQ(std::string{CPR_VERSION}, expected);
}

TEST(VersionTests, VersionNumMatchesParts) {
    const uint64_t expected = (static_cast<uint64_t>(CPR_VERSION_MAJOR) << 16) | (static_cast<uint64_t>(CPR_VERSION_MINOR) << 8) | static_cast<uint64_t>(CPR_VERSION_PATCH);
    EXPECT_EQ(CPR_VERSION_NUM, expected);
}

#if defined(CPR_ENABLE_SSL)
#if defined(CPR_FORCE_OPENSSL_BACKEND)
TEST(VersionTests, OpenSSLBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    const std::string backend{version->ssl_version};
    // OpenSSL-compatible libraries also use curl's OpenSSL backend.
    EXPECT_TRUE(backend.find("OpenSSL/") == 0 || backend.find("LibreSSL/") == 0 ||
                backend.find("BoringSSL") == 0 || backend.find("AWS-LC/") == 0 ||
                backend.find("quictls/") == 0) << backend;
}
#endif

#if defined(CPR_FORCE_WINSSL_BACKEND)
TEST(VersionTests, WinSSLBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    EXPECT_EQ(std::string{version->ssl_version}.find("Schannel"), 0);
}
#endif

#if defined(CPR_FORCE_DARWINSSL_BACKEND)
TEST(VersionTests, DarwinSSLBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    EXPECT_EQ(std::string{version->ssl_version}.find("SecureTransport"), 0);
}
#endif

#if defined(CPR_FORCE_MBEDTLS_BACKEND)
TEST(VersionTests, MbedTLSBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    EXPECT_EQ(std::string{version->ssl_version}.find("mbedTLS/"), 0);
}
#endif

#if defined(CPR_FORCE_WOLFSSL_BACKEND)
TEST(VersionTests, WolfSSLBackendUsed) {
    const curl_version_info_data* version = curl_version_info(CURLVERSION_NOW);
    ASSERT_NE(version, nullptr);
    ASSERT_NE(version->ssl_version, nullptr);
    EXPECT_NE(version->features & CURL_VERSION_SSL, 0);
    EXPECT_EQ(std::string{version->ssl_version}.find("wolfSSL/"), 0);
}
#endif
#endif // CPR_ENABLE_SSL


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
