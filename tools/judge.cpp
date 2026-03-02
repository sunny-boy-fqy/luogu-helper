#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <poll.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <errno.h>
#include <string.h>
#include <sstream>

using namespace std;

enum JudgeResult { AC, WA, TLE, RE, TOTAL_AC };

struct TestResult {
    int id;
    JudgeResult result;
    long long time_ms;
    string message;
    string userAnswer;
    string correctAnswer;
};

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    if (start == string::npos) return "";
    
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return s.substr(start, end - start + 1);
}

string readFileContent(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    if (size <= 0) {
        file.close();
        return "";
    }
    
    file.seekg(0, ios::beg);
    vector<char> buffer(size);
    file.read(buffer.data(), size);
    file.close();
    
    string content(buffer.begin(), buffer.end());
    
    size_t end = content.find_last_not_of(" \n\r\t\f\v");
    if (end != string::npos) {
        content = content.substr(0, end + 1);
    } else {
        content.clear();
    }
    
    return content;
}

bool compareFiles(const string& inputFile, const string& userOutput, const string& expectedOutput, string& contentUser, string& contentExpected) {
    // If a special judge exists, run it
    const string spjPath = string("data/spj");
    if (access(spjPath.c_str(), X_OK) == 0) {
        // run spj: spj input userOutput correctOutput
        string cmd = spjPath + " " + inputFile + " " + userOutput + " " + expectedOutput;
        int ret = system(cmd.c_str());
        if (WIFEXITED(ret)) {
            int exitCode = WEXITSTATUS(ret);
            contentUser = readFileContent(userOutput);
            contentExpected = readFileContent(expectedOutput);
            return exitCode == 0;
        }
    }

    contentUser = readFileContent(userOutput);
    contentExpected = readFileContent(expectedOutput);

    return contentUser == contentExpected;
}

TestResult runSingleTest(const string& exePath, int testId, int timeLimit) {
    TestResult res;
    res.id = testId;
    
    string inputFile = string("data/") + "data" + to_string(testId)+".in";
    string outputFile = string("data/") + "data" + to_string(testId)+".out";
    string userOutput = string("data/") + "user_out_" + to_string(testId);

    // configuration constants (could be moved to global config)
    const long MAX_MEMORY_MB = 256; // per-process address space limit
    const long MAX_OUTPUT_MB = 50; // limit output file size to read
    const int MAX_OPEN_FILES = 64;
    const long MAX_STACK_MB = 8;

    struct rusage usage {};
    auto start = chrono::high_resolution_clock::now();

    pid_t pid = fork();
    if (pid == -1) {
        res.result = WA;
        res.message = "Failed to fork";
        res.time_ms = 0;
        return res;
    }

    if (pid == 0) {
        // Child: set up resource limits and redirect stdin/stdout, then exec
        // Ensure child is killed if parent dies
        prctl(PR_SET_PDEATHSIG, SIGKILL);

        // setrlimit: address space
        struct rlimit rl;
        rl.rlim_cur = rl.rlim_max = (rlim_t)(MAX_MEMORY_MB * 1024 * 1024);
        setrlimit(RLIMIT_AS, &rl);

        // setrlimit: CPU time (seconds) - give a small cushion
        rl.rlim_cur = rl.rlim_max = (rlim_t)((timeLimit / 1000) + 1);
        setrlimit(RLIMIT_CPU, &rl);

        // setrlimit: file size
        rl.rlim_cur = rl.rlim_max = (rlim_t)(MAX_OUTPUT_MB * 1024 * 1024);
        setrlimit(RLIMIT_FSIZE, &rl);

        // setrlimit: number of open files
        rl.rlim_cur = rl.rlim_max = (rlim_t)MAX_OPEN_FILES;
        setrlimit(RLIMIT_NOFILE, &rl);

        // setrlimit: stack size
        rl.rlim_cur = rl.rlim_max = (rlim_t)(MAX_STACK_MB * 1024 * 1024);
        setrlimit(RLIMIT_STACK, &rl);

        // disable core dumps
        rl.rlim_cur = rl.rlim_max = 0;
        setrlimit(RLIMIT_CORE, &rl);

        // redirect stdin/stdout
        int in_fd = open(inputFile.c_str(), O_RDONLY);
        if (in_fd == -1) {
            _exit(127);
        }
        int out_fd = open(userOutput.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd == -1) {
            close(in_fd);
            _exit(127);
        }

        if (dup2(in_fd, STDIN_FILENO) == -1) _exit(127);
        if (dup2(out_fd, STDOUT_FILENO) == -1) _exit(127);

        close(in_fd);
        close(out_fd);

        // Execute the program (no arguments)
        char *const argv[] = { const_cast<char*>(exePath.c_str()), NULL };
        execv(exePath.c_str(), argv);

        // If execv returns, fail
        _exit(127);
    }

    // Parent: use signalfd + timerfd + poll to avoid busy-waiting
    int sigfd = -1;
    int soft_tfd = -1;
    int hard_tfd = -1;
    int status = 0;
    bool soft_expired = false;
    bool hard_expired = false;
    bool reaped = false;

    // block SIGCHLD and create signalfd
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == 0) {
        sigfd = signalfd(-1, &mask, SFD_CLOEXEC | SFD_NONBLOCK);
    }

    // create timerfds for soft and hard limits
    soft_tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    hard_tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    struct itimerspec soft_spec{};
    struct itimerspec hard_spec{};
    long long limit_ms = timeLimit;
    long long hard_limit_ms = timeLimit * 2LL;
    soft_spec.it_value.tv_sec = limit_ms / 1000;
    soft_spec.it_value.tv_nsec = (limit_ms % 1000) * 1000000;
    hard_spec.it_value.tv_sec = hard_limit_ms / 1000;
    hard_spec.it_value.tv_nsec = (hard_limit_ms % 1000) * 1000000;
    if (soft_tfd != -1) timerfd_settime(soft_tfd, 0, &soft_spec, NULL);
    if (hard_tfd != -1) timerfd_settime(hard_tfd, 0, &hard_spec, NULL);

    struct pollfd pfds[3];
    int pcount = 0;
    if (sigfd != -1) { pfds[pcount].fd = sigfd; pfds[pcount].events = POLLIN; pcount++; }
    if (soft_tfd != -1) { pfds[pcount].fd = soft_tfd; pfds[pcount].events = POLLIN; pcount++; }
    if (hard_tfd != -1) { pfds[pcount].fd = hard_tfd; pfds[pcount].events = POLLIN; pcount++; }

    while (!reaped && !hard_expired) {
        int ret = poll(pfds, pcount, -1);
        if (ret <= 0) continue;

        // check signalfd
        if (sigfd != -1 && pfds[0].fd == sigfd && (pfds[0].revents & POLLIN)) {
            struct signalfd_siginfo fdsi;
            ssize_t s = read(sigfd, &fdsi, sizeof(fdsi));
            (void)s;
            // try to reap child
            pid_t w = wait4(pid, &status, WNOHANG, &usage);
            if (w == pid) reaped = true;
        }

        // check soft timer
        for (int i = 0; i < pcount; ++i) {
            if (pfds[i].fd == soft_tfd && (pfds[i].revents & POLLIN)) {
                uint64_t expirations;
                read(soft_tfd, &expirations, sizeof(expirations));
                if (!soft_expired) {
                    kill(pid, SIGTERM);
                    soft_expired = true;
                }
            }
            if (pfds[i].fd == hard_tfd && (pfds[i].revents & POLLIN)) {
                uint64_t expirations;
                read(hard_tfd, &expirations, sizeof(expirations));
                kill(pid, SIGKILL);
                hard_expired = true;
                // attempt to reap
                pid_t w = wait4(pid, &status, WNOHANG, &usage);
                if (w == pid) reaped = true;
            }
        }

        // also attempt to reap in each loop
        if (!reaped) {
            pid_t w = wait4(pid, &status, WNOHANG, &usage);
            if (w == pid) reaped = true;
        }
    }

    // cleanup fds
    if (sigfd != -1) close(sigfd);
    if (soft_tfd != -1) close(soft_tfd);
    if (hard_tfd != -1) close(hard_tfd);

    // determine CPU time from rusage if available
    long long cpu_ms = 0;
    if (usage.ru_utime.tv_sec || usage.ru_utime.tv_usec || usage.ru_stime.tv_sec || usage.ru_stime.tv_usec) {
        cpu_ms = (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1000LL +
                 (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1000LL;
        res.time_ms = cpu_ms;
    } else {
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        res.time_ms = duration.count();
    }

    if (hard_expired) {
        res.result = TLE;
        res.message = "Terminated (2x time limit)";
        res.userAnswer = "";
        res.correctAnswer = "";
        return res;
    }

    if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        // If killed by our SIGKILL or SIGTERM due to timeout, report TLE
        if (sig == SIGKILL || sig == SIGTERM) {
            res.result = TLE;
            res.message = string("Time limit exceeded (signal ") + to_string(sig) + ")";
            res.userAnswer = "";
            res.correctAnswer = "";
            return res;
        }

        // Common runtime error signals -> RE
        if (sig == SIGSEGV || sig == SIGABRT || sig == SIGFPE || sig == SIGBUS || sig == SIGILL) {
            res.result = RE;
            res.message = string("Runtime error (signal ") + to_string(sig) + ")";
            res.userAnswer = "";
            res.correctAnswer = "";
            return res;
        }

        // Other signals -> WA by default
        res.result = WA;
        res.message = string("Terminated by signal: ") + to_string(sig);
        res.userAnswer = "";
        res.correctAnswer = "";
        return res;
    }

    if (WIFEXITED(status)) {
        int exitCode = WEXITSTATUS(status);
        if (exitCode != 0) {
            // execv failure or other exec errors -> RE
            if (exitCode == 127) {
                res.result = RE;
                res.message = string("Execution failed (code ") + to_string(exitCode) + ")";
            } else {
                res.result = WA;
                res.message = string("Program exited with code: ") + to_string(exitCode);
            }
            res.userAnswer = "";
            res.correctAnswer = "";
            return res;
        }
    }

    // If reached here, program exited with code 0 within time
    string userAnswer, correctAnswer;
    // Pre-check: ensure expected output file exists
    if (access(outputFile.c_str(), R_OK) != 0) {
        res.result = WA;
        res.message = "Missing correct output file";
        return res;
    }

    bool match = compareFiles(inputFile, userOutput, outputFile, userAnswer, correctAnswer);

    res.userAnswer = userAnswer;
    res.correctAnswer = correctAnswer;

    if (match) {
        res.result = AC;
        res.message = "Accepted";
    } else {
        res.result = WA;
        res.message = "Wrong answer";
    }
    
    
    return res;
}

int main() {
    ifstream info_file("data/info");
    if (!info_file.is_open()) {
        info_file.open("info");
    }
    if (!info_file.is_open()) {
        cout << "Cannot open info" << endl;
        return 1;
    }
    int n, timeLimit;
    info_file >> n >> timeLimit;
    info_file.close();

    // 2. Normal judging process
    system("rm main 2>/dev/null");
    if (system("g++ main.cpp -std=c++14 -O2 -o main") != 0) {
        cout << "Compilation Error!" << endl;
        return 1;
    }
    string exePath="main"; // Path to the executable to judge
    
    cout << "Number of test cases: " << n << ", Time limit: " << timeLimit << "ms" << endl;
    cout << "==================================" << endl;
    
    vector<TestResult> results;
    JudgeResult finalResult = TOTAL_AC;
    int firstNonAC = -1;
    TestResult firstNonACResult;
    
    for (int i = 1; i <= n; i++) {
        cout << "Test case " << i << ": ";
        
        TestResult res = runSingleTest(exePath, i, timeLimit);
        results.push_back(res);
        
        switch (res.result) {
            case AC:
                cout << "AC (" << res.time_ms << "ms)" << endl;
                break;
            case WA:
                cout << "WA (" << res.message << ")" << endl;
                break;
            case RE:
                cout << "RE (" << res.message << ")" << endl;
                break;
            case TLE:
                cout << "TLE (" << res.message << ", " << res.time_ms << "ms)" << endl;
                break;
        }
        
        if (finalResult == TOTAL_AC && res.result != AC) {
            finalResult = res.result;
            firstNonAC = i;
            firstNonACResult = res;
        }
    }
    
    cout << "==================================" << endl;
    
    if (finalResult == TOTAL_AC) {
        cout << "Final result: AC (All test cases passed)" << endl;
    } else {
        cout << "Final result: ";
        switch (finalResult) {
            case WA:
                cout << "WA";
                break;
            case RE:
                cout << "RE";
                break;
            case TLE:
                cout << "TLE";
                break;
            default:
                cout << "";
                break;
        }
        cout << " (First error at test case: " << firstNonAC << ")" << endl;
        
        if (finalResult == WA || finalResult == RE) {
            cout << "\nFirst WA at test case " << firstNonAC << ":" << endl;
            
            string inputFile = string("data/") + "data" + to_string(firstNonAC) + ".in";
            string inputData = readFileContent(inputFile);
            if (!inputData.empty()) {
                cout << "Input: " << inputData << endl;
            }
            
            cout << "Your answer:    " << firstNonACResult.userAnswer << endl;
            cout << "Correct answer: " << firstNonACResult.correctAnswer << endl;
            
            if (firstNonACResult.userAnswer.empty()) {
                cout << "Note: Your program produced no output!" << endl;
            }
        }
    }
    
    return 0;
}