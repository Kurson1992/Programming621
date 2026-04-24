
           MOODLEY's BANK MULTI BRANCH SYSTEM

PROJECT OVERVIEW
This C++ Banking System is designed to simulate core banking 
operations including customer registration, multi-branch 
management, transaction processing (Deposit, Withdrawal, 
Interest), and simple XOR encryption for PIN security.

CLION SETUP INSTRUCTIONS
To run this project in CLion:
1. Open CLion and select 'New Project' -> C++ Executable.
2. Copy and paste the code into the 'main.cpp' file.
3. Go to 'File' -> 'Reload CMake Project'.
4. Click the green 'Run' arrow in the top right corner

DEFAULT LOGIN CREDENTIALS
The system auto-seeds a default teller if 'tellers.dat' is not 
found. Use these credentials for the first login:
- Teller ID: T001
- Password:  1234

SYSTEM FEATURES & USAGE
- TELLER MODE: 
  Allows registration of new customers. The 5-digit PIN is 
  generated automatically and displayed ONCE.
- CUSTOMER MODE (ATM):
  Allows customers to login using their Account Number and PIN
  to view balances, withdraw, or deposit funds.
- SECURIT
  - 3-Attempt Lockout: Customers are locked out after 3 
    failed PIN attempts.






============================================================
