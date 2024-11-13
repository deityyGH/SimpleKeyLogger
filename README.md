# SimpleKeyLogger

This is a simple Windows keylogger implemented with Windows API in C++ (client) and Python (server). The C++ client captures keystrokes and sends them to a Flask server (Python), which logs them for demonstration purposes. This project is meant purely as a proof of concept and should not be used for malicious purposes.

## **WARNING**
- **This software is created for educational purposes only.** Unauthorized use of a keylogger to record another person’s keystrokes without consent is illegal.
- The keylogger may be detected by antivirus programs, including Windows Defender, as it operates similarly to malicious software. Use only in controlled environments or virtual machines where you have explicit permission.

## Features

- C++ Client: Logs keystrokes and sends them to the Flask server in real-time.
- Python (Flask) Server: Receives keystroke data and logs it to a file or displays it in the console.


## Usage
1. Modify the IP Address
    Update the `x.x.x.x` IP address in both the C++ and Python files:

2. Start the Flask Server
    Run the Python file to start the Flask server, which will listen for incoming keystroke data.

3. Start the Client
    After starting the server, run the C++ client. As you type, keystrokes will be sent and logged in real-time on the Flask server.

4. Enable Temp and/or registry persistence (optional)
    - Specify `--persistence` argument to save the executable to temp and add it to registry to start on boot.

## Important Note
This project is intended solely for educational purposes, to demonstrate the basics of how a keylogger works in C++ and Python. **I will not be providing a step-by-step tutorial on how to use this software.** This is to discourage misuse, as keyloggers can be abused for malicious purposes. Only individuals with prior experience should attempt to run this code, and it should be done responsibly and within legal boundaries, such as on your own systems for testing or research.

## License
This project is licensed under the MIT License - see the [LICENSE](https://github.com/deityyGH/SimpleKeyLogger/blob/main/LICENSE) file for details.

## Legal Disclaimer
This project is for educational purposes only. Unauthorized use of keyloggers to record personal information without consent is a **violation of privacy laws** and **strictly prohibited**. By using this code, you agree to take **full responsibility** for the legality of your actions.