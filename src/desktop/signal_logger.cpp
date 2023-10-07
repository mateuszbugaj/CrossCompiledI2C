#include <signal_logger.hpp>
#include <vector>
#include <fstream>

void SL_logPinState(HAL_Pin* signals, uint8_t size) {
    std::ofstream file(OUTPUT_FILE, std::ios_base::app);
    if (file.is_open()) {
        for(int i = 0; i < size; i++){
          file << (int)signals[i].level << " ";
        }

        file << '\n';
        file.close();
    }
}
