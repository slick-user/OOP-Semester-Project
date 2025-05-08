#include "MFA.h"
#include <iostream>
#include <cstdlib>

MFA::MFA(const char* user) {
    strncpy(username, user, sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';
    generateOtpFilePath();
}

MFA::~MFA() {
    cleanupExpiredOtp();
}

void MFA::generateOtpFilePath() {
    snprintf(otpFilePath, sizeof(otpFilePath), "%s_otp.txt", username);
}

bool MFA::generateAndStoreOTP() {
    cleanupExpiredOtp();  // Clean any existing expired OTP

    // Generate new 6-digit OTP
    char otp[OTP_LENGTH + 1];
    srand(static_cast<unsigned int>(time(0)));
    for(int i = 0; i < OTP_LENGTH; i++) {
        otp[i] = '0' + (rand() % 10);
    }
    otp[OTP_LENGTH] = '\0';

    // Store OTP with timestamp
    std::ofstream otpFile(otpFilePath);
    if (!otpFile.is_open()) return false;

    time_t generationTime = time(0);
    otpFile << otp << "\n";
    otpFile << generationTime << "\n";
    otpFile.close();

    return true;
}

bool MFA::validateOTP(const char* inputOTP) {
    if (!isOtpFileValid()) return false;

    std::ifstream otpFile(otpFilePath);
    if (!otpFile.is_open()) return false;

    char storedOTP[OTP_LENGTH + 1];
    time_t generationTime;

    otpFile >> storedOTP >> generationTime;
    otpFile.close();

    // Check expiration
    if (difftime(time(0), generationTime) > OTP_VALIDITY_SECONDS) {
        cleanupExpiredOtp();
        return false;
    }

    // Validate OTP
    bool isValid = (strcmp(inputOTP, storedOTP) == 0);
    if (isValid) {
        cleanupExpiredOtp();  // Remove OTP file after successful validation
    }
    return isValid;
}

bool MFA::isOtpExpired() const {
    if (!isOtpFileValid()) return true;

    std::ifstream otpFile(otpFilePath);
    if (!otpFile.is_open()) return true;

    char storedOTP[OTP_LENGTH + 1];
    time_t generationTime;

    otpFile >> storedOTP >> generationTime;
    otpFile.close();

    return difftime(time(0), generationTime) > OTP_VALIDITY_SECONDS;
}

bool MFA::isOtpFileValid() const {
    std::ifstream otpFile(otpFilePath);
    if (!otpFile.is_open()) return false;

    char storedOTP[OTP_LENGTH + 1];
    time_t generationTime;

    if (!(otpFile >> storedOTP >> generationTime)) {
        otpFile.close();
        return false;
    }

    otpFile.close();
    return true;
}

void MFA::cleanupExpiredOtp() const {
    if (isOtpExpired()) {
        remove(otpFilePath);
    }
}

time_t MFA::getRemainingTime() const {
    if (!isOtpFileValid()) return 0;

    std::ifstream otpFile(otpFilePath);
    if (!otpFile.is_open()) return 0;

    char storedOTP[OTP_LENGTH + 1];
    time_t generationTime;

    otpFile >> storedOTP >> generationTime;
    otpFile.close();

    time_t remaining = OTP_VALIDITY_SECONDS - difftime(time(0), generationTime);
    return remaining > 0 ? remaining : 0;
}