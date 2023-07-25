from socket import *

def start_server(page):
    try:
        server = socket(AF_INET, SOCK_STREAM)
        server.bind(('127.0.0.1', 3000))
        server.listen(2)
        while True:
            client_socket, addr = server.accept()
            data = client_socket.recv(1024).decode('utf-8')
            print(data)
            content = load_get_request(data)
            client_socket.send(content)
            client_socket.shutdown(SHUT_WR)
    except KeyboardInterrupt:
        server.close()

def load_get_request(data):
    HDRS = 'HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n'
    HDRS404 = 'HTTP/1.1 404 ERROR\r\nContent-Type: text/html; charset=utf-8\r\n\r\n'

    path = data.split(' ')[1]
    response = ''
    try:
        with open('templates' + path, 'rb') as file:
            response = HDRS.encode('utf-8') + file.read()
    except FileNotFoundError:
        response = (HDRS404 + "Damn...").encode('utf-8')
    return response

if __name__ == "__main__":
    start_server('youtube')