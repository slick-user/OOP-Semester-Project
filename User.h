#ifndef USER_H
#define USER_H

#include <string>
#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace std;

class User {
protected:
    string name;
    string password;
    bool isAuthenticated;
    string otp;
    time_t otpGeneratedAt;

public:
    User(const string& name, const string& pass);
    virtual ~User();

    virtual int getClearanceLevel() const = 0;
    virtual string getRoleName() const = 0;

    string getName() const;
    bool authenticate(const string& enteredPassword);
    void generateOtp();
    bool validateOtp(const string& enteredOtp);
    void sendOtpMessage();
};

#endif
