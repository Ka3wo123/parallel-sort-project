#include <string>
#include <iomanip>
#include <sstream>

std::string formatTime(double seconds) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    
    if (seconds < 0.001) {
        ss << seconds * 1000000.0 << " us";
    } else if (seconds < 1.0) {
        ss << seconds * 1000.0 << " ms";  
    } else if (seconds < 60.0) {
        ss << seconds << " s";            
    } else {
        int minutes = static_cast<int>(seconds) / 60;
        double remSeconds = seconds - (minutes * 60);
        ss << minutes << " min " << remSeconds << " s";
    }
    return ss.str();
}