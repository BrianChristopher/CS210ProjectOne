#include <iostream>
#include <string>
#include <ctime>
#include <thread>

using namespace std;
using namespace std::chrono; 

string Format24HrTime(int hour24In, int minuteIn, int secondIn) {
    string hour = to_string(hour24In);
    string minute = to_string(minuteIn);
    string second = to_string(secondIn);

    if (hour.length() == 1) {
        hour = "0" + hour;
    }
    if (minute.length() == 1) {
        minute = "0" + minute;
    }
    if (second.length() == 1) {
        second = "0" + second;
    }

    return hour + ":" + minute + ":" + second;
}

string Format12HrTime(int hour12In, int minuteIn, int secondIn, bool isPMin) {
    string hour = to_string(hour12In);
    string minute = to_string(minuteIn);
    string second = to_string(secondIn);
    string amPm = isPMin ? "PM" : "AM";

    if (hour.length() == 1) {
        hour = "0" + hour;
    }
    if (minute.length() == 1) {
        minute = "0" + minute;
    }
    if (second.length() == 1) {
        second = "0" + second;
    }

    return hour + ":" + minute + ":" + second + " " + amPm;
}

void chadaTechClockDisplay(string time12Hour, string time24Hour ) {
    string firstLine    = "  **************************     **************************";
    string secondLine   = "  *      12-Hour Clock     *     *      24-Hour Clock     *";
    string thirdLine    = "  *       " + time12Hour + "      *     *        " + time24Hour + "        *";
    string fourthLine   = "  **************************     **************************";
    string fifthLine    = "  **************************";
    string sixthLine    = "  * 1 - Add One Hour       *";
    string seventhLine  = "  * 2 - Add One Minute     *";
    string eighthLine   = "  * 3 - Add One Second     *";
    string ninthLine    = "  * 4 - Exit Program       *";
    string tenthLine    = "  **************************";

    cout << firstLine << endl;
    cout << secondLine << endl;
    cout << thirdLine << endl;
    cout << fourthLine << endl;
    cout << endl;
    cout << fifthLine << endl;
    cout << sixthLine << endl;
    cout << seventhLine << endl;
    cout << eighthLine << endl;
    cout << ninthLine << endl;
    cout << tenthLine << endl;

    return;
}

void getCurrentTime(int& hour24, int& minute, int& second) {
    struct tm currentTime;
    time_t now = time(0);       
    localtime_s(&currentTime, &now);

    hour24 = currentTime.tm_hour;
    minute = currentTime.tm_min;
    second = currentTime.tm_sec;

    return;
}

void set12HourClockVariables(int& hour24Now, int& hour12Now, bool& isPM) {
    if (hour24Now == 0) {
        hour12Now = 12;
        isPM = false;
    }
    else if (hour24Now < 12) {
        hour12Now = hour24Now;
        isPM = false;
    }
    else if (hour24Now == 12) {
        hour12Now = 12;
        isPM = true;
    }
    else {
        hour12Now = hour24Now - 12;
        isPM = true;
    }
    
    return;
}

void addSecondToTime(int& hour24Now, int& hour12Now, int& minuteNow, int& secondNow, bool& isPM) {
    secondNow = secondNow + 1;
    if (secondNow == 60) {
        secondNow = 0;
        minuteNow = minuteNow + 1;
    }
    if (minuteNow == 60) {
        minuteNow = 0;
        hour24Now = hour24Now + 1;
    }
    if (hour24Now == 24) {
        hour24Now = 0;
    }
    set12HourClockVariables(hour24Now, hour12Now, isPM);
}

void addMinuteToTime(int& hour24Now, int& hour12Now, int& minuteNow, bool& isPM) {
    minuteNow = minuteNow + 1;
    if (minuteNow == 60) {
        minuteNow = 0;
        hour24Now = hour24Now + 1;
    }
    if (hour24Now == 24) {
        hour24Now = 0;
    }
    set12HourClockVariables(hour24Now, hour12Now, isPM);
}

void addHourToTime(int& hour24Now, int& hour12Now, bool& isPM) {
    hour24Now = hour24Now + 1;
    if (hour24Now == 24) {
        hour24Now = 0;
    }
    set12HourClockVariables(hour24Now, hour12Now, isPM);
}

// This is the function called by the simultaneous thread ("first" thread) within Main
// It is responsible for adding a second to clock and then updating display.
void updateScreen(bool& exitProgram, int& hour24Now, int& hour12Now, int& minuteNow, int& secondNow, 
    bool& isPM, string& time12HrFormat, string& time24HrFormat) {

    while (!exitProgram) {
        // Add second to second variable
        addSecondToTime(hour24Now, hour12Now, minuteNow, secondNow, isPM);

        //Pause for one second
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        // Format the time for output
        time24HrFormat = Format24HrTime(hour24Now, minuteNow, secondNow);
        time12HrFormat = Format12HrTime(hour12Now, minuteNow, secondNow, isPM);
        
        // Display output
        system("CLS");
        chadaTechClockDisplay(time12HrFormat, time24HrFormat);   
    }
    return;
}


int main() {
    
    // main variables
    bool exitProgram = false;
    char userSelection;
    int hour24Now;
    int hour12Now;
    int minuteNow;
    int secondNow;
    bool isPM;
    string time12HrFormat;
    string time24HrFormat;

    // Get current time and initialize all time variables
    getCurrentTime(hour24Now, minuteNow, secondNow);
    set12HourClockVariables(hour24Now, hour12Now, isPM);

    // Format the time for output
    time24HrFormat = Format24HrTime(hour24Now, minuteNow, secondNow);
    time12HrFormat = Format12HrTime(hour12Now, minuteNow, secondNow, isPM);

    // Display output
    chadaTechClockDisplay(time12HrFormat, time24HrFormat);

    // START SCREEN UPDATE THREAD
    // This thread is responsible for adding a second to current second, then updating screen.
    auto first = std::thread(updateScreen, 
            std::ref(exitProgram),
            std::ref(hour24Now),
            std::ref(hour12Now),
            std::ref(minuteNow),
            std::ref(secondNow),
            std::ref(isPM),
            std::ref(time12HrFormat), 
            std::ref(time24HrFormat));

    // Handles user input and menu selection
    while (!exitProgram) {
        cin >> userSelection;
        switch (userSelection) {
            case '1': {
                addHourToTime(hour24Now, hour12Now, isPM);
                break;
            }
            case '2': {
                addMinuteToTime(hour24Now, hour12Now, minuteNow, isPM);
                break;
            }
            case '3': {
                addSecondToTime(hour24Now, hour12Now, minuteNow, secondNow, isPM);
                break;
            }
            case '4': {
                exitProgram = true;
                cout << "Exiting program. Goodbye.";
                break;
            }
            default: {
                break;
            }
        }
    }
        first.join(); // Needed to join/end "first" thread

    return 0;
}
