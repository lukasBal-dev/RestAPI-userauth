This is a REST API built using C++ and the cpprestsdk library, designed for handling user registration and login functionality. The API performs password hashing and securely stores user data in a JSON file.
Features

User Registration: Allows new users to sign up by providing a username and password.

User Login: Authenticates users by verifying the provided username and password.

Password Hashing: Passwords are securely hashed before storing to ensure better security.

Data Storage: User data is stored in a JSON file, making it easy to update and manage.

API Endpoints
1. Register a New User

To register a new user, send a POST request with the following JSON body:

    {
      "reg": {
        "username": "your_username",
        "password": "your_password"
      }
    }

Response:

On success: "message": "User registered successfully"

If the username already exists: "error": "Username already exists"

2. Login User

To log in, send a POST request with the following JSON body:

    {
      "log": {
        "username": "your_username",
        "password": "your_password"
      }
    }

Response:

On success: "success": "User logged in"

If the username does not exist or the password is incorrect: "error": "Wrong password" or "error": "No user found!"

Supported Methods

GET: Used for a simple health check or greeting message.

Example response: 
    
    {"message": ":)"}

POST: Used for both user registration and login requests.

How It Works

The API accepts incoming POST requests with user data (either for registration or login).
Password Hashing: The passwords are hashed using a simple hashing function and are stored as a hashed value in the users.json file.
Error Handling: The API ensures appropriate error messages for cases like:
Username already exists.
Wrong password.
Missing or invalid fields in the request body.

Technologies Used

1. C++: For the main logic and handling the API requests.

2. cpprestsdk: A C++ library to help build REST APIs with support for HTTP client and server functionalities.

3. JSON: To store and manage user data securely in a readable format.

Getting Started
Prerequisites

To build and run this project, you need the following:

A C++ compiler (e.g., GCC, Clang, or MSVC).

cpprestsdk library installed.

You can install it via cpprestsdk GitHub.

Building the Project

Clone this repository to your local machine.

Open the project in Visual Studio or your preferred IDE.

Build the project to generate the executable.

Running the API

Once the API is built, run the executable. The server will start and listen for incoming requests on http://localhost:8080.
Testing the API

You can test the API using tools like Postman, curl, or any HTTP client.

Example curl commands:

Register User:

    curl -X POST http://localhost:8080 -H "Content-Type: application/json" -d '{"reg":{"username":"testuser", "password":"testpass"}}'

Login User:

    curl -X POST http://localhost:8080 -H "Content-Type: application/json" -d '{"log":{"username":"testuser", "password":"testpass"}}'

Contributing

If you'd like to contribute to this project, feel free to fork the repository and submit a pull request. Any improvements or suggestions are welcome!
License

This project is licensed under the MIT License - see the LICENSE file for details.
