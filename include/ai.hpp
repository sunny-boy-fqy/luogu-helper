#ifndef AI_HPP
#define AI_HPP

#include <string>
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <vector>
#include <cstdlib>

namespace ai {

/**
 * @brief Executes a shell command and returns the output.
 */
inline std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    // Using popen to capture stdout
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

/**
 * @brief Simple query to the AI.
 * @param prompt The prompt to send.
 * @param quiet Whether to suppress status messages (uses -q).
 * @return The AI's response.
 */
inline std::string query(const std::string& prompt, bool quiet = true) {
    // Escape single quotes for shell safety
    std::string escaped_prompt;
    for (char c : prompt) {
        if (c == '\'') {
            escaped_prompt += "'\\''";
        } else {
            escaped_prompt += c;
        }
    }
    
    std::string command = "/home/fangqiyu/BIGMODEL/ai_run.sh ";
    if (quiet) {
        command += "-q ";
    }
    command += "'" + escaped_prompt + "'";
    
    return exec(command);
}

/**
 * @brief Enters interactive chat mode.
 */
inline void chat() {
    std::system("/home/fangqiyu/BIGMODEL/ai_run.sh chat");
}

} // namespace ai

#endif // AI_HPP