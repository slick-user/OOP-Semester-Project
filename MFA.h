#ifndef MFA_H
#define MFA_H

#include <ctime>
#include <cstring>
#include <fstream>

class MFA {
private:
    static const int OTP_LENGTH = 6;
    static const int OTP_VALIDITY_SECONDS = 300; // 5 minutes
    
    char username[50];
    char otpFilePath[100];

    void generateOtpFilePath();
    bool isOtpFileValid() const;
    void cleanupExpiredOtp() const;

public:
    explicit MFA(const char* user);
    ~MFA();

    bool generateAndStoreOTP();
    bool validateOTP(const char* inputOTP);
    bool isOtpExpired() const;
    time_t getRemainingTime() const;
};

#endif