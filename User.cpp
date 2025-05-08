#include "User.h"

User::User(const string& name, const string& pass)
    : name(name), password(pass), isAuthenticated(false) {}

User::~User() {}

string User::getName() const {
    return name;
}

bool User::authenticate(const string& enteredPassword) {
    if (enteredPassword == password) {
        generateOtp();
        sendOtpMessage();
        return true;
    }
    return false;
}

void User::generateOtp() {
    srand(time(0));
    otp = to_string(rand() % 900000 + 100000);
    otpGeneratedAt = time(0);
}

bool User::validateOtp(const string& enteredOtp) {
    if (difftime(time(0), otpGeneratedAt) > 300) {
        return false;
    }
    if (otp == enteredOtp) {
        isAuthenticated = true;
        return true;
    }
    return false;
}

void User::sendOtpMessage() {
    cout << "Sending OTP to " << name << ": " << otp << endl;
}