# Custom In-Memory Network Database

A simple networked key-value database written n C++. 
This project was built to explore low-level OS networking, memory management, and database architecture.

## How to Run It

### 1. Compile and Start the Server
**On Mac / Linux:**
```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git
cd YOUR_REPO_NAME
g++ -std=c++17 servermain.cpp directory.cpp -o server
./server
```

**On Windows:**
Simply open the folder in Visual Studio, ensure the ws2_32.lib pragma is active in servermain.cpp, and click "Build and Run".

### 2. Connect to the Database
Once the server says Listening on port 8080..., open a second terminal window and connect using Netcat (Mac/Linux) or Telnet (Windows):
code
```bash
nc 127.0.0.1 8080
```

### 3. Try these Commands!
Commands:
SET	
GET	
INCR	
COPY	
DEL	
SAVE	
LOAD	
EXIT