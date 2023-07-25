from socket import *

client = socket(
    AF_INET, SOCK_STREAM
)

client.connect(('127.0.0.1', 7000))

while True:
    inp = input("Client: ")
    client.send(inp.encode("utf-8"))
    data = client.recv(1024)
    msg = data.decode("utf-8")
    if inp == "exit":
        break
    print(f"Server: {msg}")

