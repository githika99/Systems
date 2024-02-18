## httpserver.c: 
creates a server for client to send GET and PUT commands to 

## Makefile: 
creates the httpserver memory

## Use:
make all - produces the httpserver binary<br>
make httpserver - produces the httpserver binary<br>
make clean - removes all .o and binary files<br>
make format - clang-formats httpserver.c<br>

## Usage:
In one terminal, set up server with ./httpserver [port_number]<br>
In another terminal, create a client that starts the connection. You can use curl, netcat, or another networking utility.<br><br>
### You can pipe to nc using: printf “your message here"| nc -N 127.0.0.1 [port_number]<br>
An example of a PUT request, that puts the contents of [message body] into foo.txt is:<br>
PUT /foo.txt HTTP/1.1\r\nContent-Length: 20\r\n\r\n [message body]<br>
An example of a GET request, that outputs the contents of foo.txt into the server is:<br>
GET /foo.txt HTTP/1.1\r\n\r\n<br><br>
### Using curl, you would make the same PUT request with this format, where messagebody.txt contains the contents of message body:<br>
curl http://localhost:1234/foo.txt -T messagebody.txt<br>
Using curl, you would make the same GET request with this format:<br>
curl http://localhost:1234/foo.txt<br>

## Testing
./test_repo.sh to run all tests<br>
./test_scripts/test_name.sh to run that particular test <br>


### Author: Githika Annapureddy
### Date: 2/05/2024
