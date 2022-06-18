# TEAME MEMBERES

LABRAYJI MOHAMED FADEL, YASSIN HADARI, BRAHIM SANAOUI

# Webserv
This project is about writing my ow HTTP server.


[//]: # (<< -------------------------- Setup Socket -------------------------------- >>)

# Socket 
  - Create an endpoint for communication
  - **Endpoint** any device that connects to a computer network.
  
## 1- Create Socket :

* int server_fd = socket(int domain, type, protocol);

* Domain, or adresse family :
    - Communication domain in which the socket should be created
    - AF_INET : IPv4 type

* Type : type of socket (STREAM, DATAGRAM ...)
    - We will use **SOCK_STREAM** that use TCP protocol
    - 
* Protocol :
    - indicate a specific protocol to use in supporting the sockets operation (if more than one , we can specified)

* Return is a fd;

* The original socket that was set up for listening is used only for accepting connections ,not for exchanging data

## 2- Identify (name) a socket: 
* Assigning a transport address to the socket (a port number in IP networking)
* In sockets, this operation is called **binding** an address and the bind system call is used for this.
* Bind actually creates a **file** in the file system.
* int bind(int socketfd, const struct sockaddr *addr, socklen_t addrlen):
    - assigns the address specified by **addr** to the **socket** referred to by the **file descriptor sockfd**

## 3- On the server, wait for incoming connection:
* int listen(int sockfd, int backlog);
    - Make the socket like a **passive socket** or acceptor socket is a type of socket that is used to to accept incoming connection requests using **accept**
    - Backlog : defines the maximum length to which the queue of pending connections for sockfd may grow.
* int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
    - Socket : was set for accepting connections with listen.
    - Address : gets filed in with the address of the client that is doing the connect(add, port..)
    - Grabs(attrape) the first connection request on the queue of pending connections (set up in listen) and creates a new socket for that connection.
    - Socket operations are **synchronous**, or **blocking**, and **accept** will block until a connection is present on the queue.

* int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict exceptfds, struct timeval *restrict timeout);

    - allows a program to monitor multiple file descriptors,
        waiting until one or more of the file descriptors become "ready"
        for some class of I/O operation.
    - select : to handle listen multiple socket
                


## 4- Send and recieve msgs:
* We finally have connected sockets between a client(when you visit IP address of your server from a web browser) and a server!
* Communication is the easy part. The same **read** and **write** system calls that work on files also work on sockets.

## 5- Close the socket:
* with close(new_socket);

[//]: # (<< ---------------------------- SELECT ---------------------------------- >>)

# Select:
    - select : destructive, changes our FD set

[//]: # (<< ---------------------------- CONFIG FILE ---------------------------------- >>)

# Config File Structure and Contexts (like ngnix)

* Context : areas that these brackets define;
    - Ex:
        context {
            . . .
            directives
        }

* Directives : server will error out on reading a configuration file with directives that are declared in the wrong context.

* The Core contexts :
    - The Main Context : context {}
    - The Event Context : events {}
    - HTTP context : http {} // In our case default is **http server**

* Server context :
    - Ex : 

    http {
        server { 
        # first server context
        }
    }
    
    - server : virtual server ,handle a specific subset of connections.
    - listen 80 0.0.0.0 : (directive) The ip address / port combination that this server block is designed to respond to
    - server_names server1 server2 : 

* The Location Context :
     location / {
            root /var/www/html;
            . . .
    }
[//]: # (<< ---------------------------- ---------------------------------- >>)

