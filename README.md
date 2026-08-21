# Custom In-Memory Network Database

A simple networked key-value database written n C++. 

## How to Run It

### 1. Compile and Start the Server
**On Mac / Linux:**
```bash
git clone https://github.com/xurbull/Custom-Network-Database.git
```
```bash
cd Custom-Network-Database
```
```bash
g++ -std=c++17 servermain.cpp directory.cpp -o server
```
```bash
./server
```

**On Windows:**
Open the folder in Visual Studio, and run it.

### 2. Connect to the Database
Once the server starts listening, open a second terminal window and connect:
```bash
nc 127.0.0.1 8080
```

### 3. Commands:
SET	
GET	
INCR	
COPY	
DEL	
SAVE	
LOAD	
EXIT
LPUSH
LRANGE