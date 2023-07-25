import threading
from socket import *

server = socket(AF_INET, SOCK_STREAM)

server.setblocking(False)

users_count = 0
shutdown_event = threading.Event()
lock = threading.Lock()

def user_receive(user):
    global users_count

    lock.acquire()
    users_count += 1
    lock.release()

    while True:
        msg = user.recv(1024).decode("utf-8")
        if not msg or msg == "exit":
            break
        print(f"Client: {msg}")
        user.send(msg.encode("utf-8"))

    lock.acquire()
    users_count -= 1
    c = users_count
    lock.release()
    if c == 0:
        server.close()
        shutdown_event.set()  
        exit()
    user.close()

server.bind(('127.0.0.1', 7000))
server.listen(2)

while not shutdown_event.is_set():
    try:
        user, addr = server.accept()
        th = threading.Thread(target=user_receive, args=(user,), daemon=True)
        th.start()
        if users_count == 0:
            break
    except BlockingIOError as e:
        pass
    

server.close()
