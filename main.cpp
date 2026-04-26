#include <thread>
#include <chrono>
#include <mach-o/dyld.h>
#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
 
typedef int (*printdef)(int, const char*, ...);
 
const char HTML_PAGE[] = 
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"    <title>printsploitv2e</title>\n"
"    <style>\n"
"        * { margin: 0; padding: 0; box-sizing: border-box; }\n"
"        body {\n"
"            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
"            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
"            min-height: 100vh;\n"
"            display: flex;\n"
"            justify-content: center;\n"
"            align-items: center;\n"
"            padding: 20px;\n"
"        }\n"
"        .container {\n"
"            background: white;\n"
"            border-radius: 10px;\n"
"            box-shadow: 0 20px 60px rgba(0,0,0,0.3);\n"
"            width: 100%;\n"
"            max-width: 600px;\n"
"            padding: 30px;\n"
"        }\n"
"        h1 {\n"
"            color: #333;\n"
"            margin-bottom: 20px;\n"
"            text-align: center;\n"
"        }\n"
"        .input-section {\n"
"            display: flex;\n"
"            gap: 10px;\n"
"            margin-bottom: 20px;\n"
"        }\n"
"        input[type=\"text\"] {\n"
"            flex: 1;\n"
"            padding: 12px;\n"
"            border: 2px solid #ddd;\n"
"            border-radius: 5px;\n"
"            font-size: 14px;\n"
"            transition: border-color 0.3s;\n"
"        }\n"
"        input[type=\"text\"]:focus {\n"
"            outline: none;\n"
"            border-color: #667eea;\n"
"        }\n"
"        select {\n"
"            padding: 12px;\n"
"            border: 2px solid #ddd;\n"
"            border-radius: 5px;\n"
"            background: white;\n"
"            cursor: pointer;\n"
"            font-size: 14px;\n"
"        }\n"
"        button {\n"
"            padding: 12px 24px;\n"
"            background: #667eea;\n"
"            color: white;\n"
"            border: none;\n"
"            border-radius: 5px;\n"
"            cursor: pointer;\n"
"            font-size: 14px;\n"
"            font-weight: bold;\n"
"            transition: background 0.3s;\n"
"        }\n"
"        button:hover {\n"
"            background: #5568d3;\n"
"        }\n"
"        .output {\n"
"            background: #1e1e1e;\n"
"            color: #0f0;\n"
"            padding: 15px;\n"
"            border-radius: 5px;\n"
"            font-family: 'Courier New', monospace;\n"
"            font-size: 13px;\n"
"            max-height: 300px;\n"
"            overflow-y: auto;\n"
"            min-height: 150px;\n"
"            border: 1px solid #333;\n"
"        }\n"
"        .log-entry {\n"
"            padding: 5px 0;\n"
"            border-bottom: 1px solid #333;\n"
"        }\n"
"        .print { color: #0f0; }\n"
"        .info { color: #0ff; }\n"
"        .warn { color: #ff0; }\n"
"        .error { color: #f00; }\n"
"        .status {\n"
"            text-align: center;\n"
"            margin-top: 15px;\n"
"            font-size: 12px;\n"
"            color: #666;\n"
"        }\n"
"        .status.connected {\n"
"            color: #0f0;\n"
"        }\n"
"    </style>\n"
"</head>\n"
"<body>\n"
"    <div class=\"container\">\n"
"        <h1>Roblox Print Console</h1>\n"
"        \n"
"        <div class=\"input-section\">\n"
"            <select id=\"colorSelect\">\n"
"                <option value=\"0\">Print (White)</option>\n"
"                <option value=\"1\">Info (Blue)</option>\n"
"                <option value=\"2\">Warn (Yellow)</option>\n"
"                <option value=\"3\">Error (Red)</option>\n"
"            </select>\n"
"            <input type=\"text\" id=\"messageInput\" placeholder=\"Type your message here...\" autofocus>\n"
"            <button onclick=\"sendMessage()\">Send</button>\n"
"        </div>\n"
"        \n"
"        <div class=\"output\" id=\"output\"></div>\n"
"        \n"
"        <div class=\"status connected\">Connected to Roblox</div>\n"
"    </div>\n"
"\n"
"    <script>\n"
"        const output = document.getElementById('output');\n"
"        const messageInput = document.getElementById('messageInput');\n"
"        const colorSelect = document.getElementById('colorSelect');\n"
"        \n"
"        const colorMap = {\n"
"            0: 'print',\n"
"            1: 'info',\n"
"            2: 'warn',\n"
"            3: 'error'\n"
"        };\n"
"        \n"
"        function addLog(color, message) {\n"
"            const entry = document.createElement('div');\n"
"            entry.className = 'log-entry ' + colorMap[color];\n"
"            entry.textContent = '[' + colorMap[color].toUpperCase() + '] ' + message;\n"
"            output.appendChild(entry);\n"
"            output.scrollTop = output.scrollHeight;\n"
"        }\n"
"        \n"
"        function sendMessage() {\n"
"            const message = messageInput.value.trim();\n"
"            if (message === '') return;\n"
"            \n"
"            const color = colorSelect.value;\n"
"            \n"
"            fetch('/', {\n"
"                method: 'POST',\n"
"                body: JSON.stringify({\n"
"                    color: color,\n"
"                    message: message\n"
"                }),\n"
"                headers: {\n"
"                    'Content-Type': 'application/json'\n"
"                }\n"
"            }).then(function() {\n"
"                addLog(color, message);\n"
"                messageInput.value = '';\n"
"                messageInput.focus();\n"
"            }).catch(function(err) {\n"
"                console.error('Error:', err);\n"
"            });\n"
"        }\n"
"        \n"
"        messageInput.addEventListener('keypress', function(e) {\n"
"            if (e.key === 'Enter') {\n"
"                sendMessage();\n"
"            }\n"
"        });\n"
"    </script>\n"
"</body>\n"
"</html>\n";
 
void httpServer(printdef printfunction)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printfunction(3, "Failed to create socket");
        return;
    }
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9999);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        printfunction(3, "Failed to bind socket");
        return;
    }
    
    listen(server_fd, 5);
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) continue;
        
        char buffer[2048];
        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer) - 1);
        
        std::string request(buffer);
        
        if (request.find("POST /") != std::string::npos) {
            // Parse JSON POST request
            size_t body_start = request.find("\r\n\r\n");
            if (body_start != std::string::npos) {
                std::string body = request.substr(body_start + 4);
                
                int color = 0;
                char message[256];
                memset(message, 0, sizeof(message));
                
                size_t color_pos = body.find("\"color\":\"") + 9;
                color = atoi(body.substr(color_pos, 1).c_str());
                
                size_t msg_pos = body.find("\"message\":\"") + 11;
                size_t msg_end = body.find("\"", msg_pos);
                std::string msg = body.substr(msg_pos, msg_end - msg_pos);
                strncpy(message, msg.c_str(), sizeof(message) - 1);
                
                if (color >= 0 && color <= 3) {
                    printfunction(color, message);
                }
                
                const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
                write(client_fd, response, strlen(response));
            }
        } else if (request.find("GET /") != std::string::npos) {
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: ";
            response += std::to_string(strlen(HTML_PAGE));
            response += "\r\n\r\n";
            response += HTML_PAGE;
            
            write(client_fd, response.c_str(), response.length());
        }
        
        close(client_fd);
    }
    
    close(server_fd);
}
 
void printSploit()
{
    std::this_thread::sleep_for(std::chrono::seconds(13));
    
    auto printfunction = reinterpret_cast<printdef>(_dyld_get_image_vmaddr_slide(0) + 0x1001c9cf8);
    httpServer(printfunction);
}
 
__attribute__((constructor)) 
void entry()
{
    std::thread(printSploit).detach();
}
