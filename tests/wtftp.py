import os
import socket
import time

BASE_DIR = os.path.dirname(os.path.realpath(__file__))

HOST = "127.0.0.1"
PORT = 4242

CRLF = b"\r\n"
BUFFER_SIZE = 1024

JUSTAFILE_CONTENT = "Hello this is just a test for WTFTP!"
IWROTETHISFILE_CONTENT = "Yeah I just wrote this and it's pretty\ngood lol!"

class BufferVerify:
    @staticmethod
    def statusCode(buffer, statusCode: int):
        return buffer.startswith(str(statusCode).encode())

    @staticmethod
    def ending(buffer) -> bool:
        return buffer.endswith(CRLF)

class Utils:
    @staticmethod
    def getPasvPort(buffer) -> Optional[int]:
        port = 0
        splitted = buffer.split(",")

        if len(splitted) != 6:
            return None

        return int(splitted[4]) * 256 + int(splitted[5].split(")")[0])

class SocketTest:
    def __init__(self, s = None):
        if s is not None:
            self.socket = s
        else:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self):
        self.socket.connect((HOST, PORT))

    # Ignore the first connection msg
    def connectAndIgnore(self):
        self.connect()
        self.socket.recv(BUFFER_SIZE)

    def close(self):
        self.socket.close()

    # Socket must be connected before this
    # Also doesn't close the socket
    def authentificate(self) -> bool:
        self.socket.send(b"USER Anonymous\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        if not BufferVerify.ending(data) and not BufferVerify.statusCode(data, 331):
            b = False
            return b

        self.socket.send(b"PASS\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 230)
        return b

    def test(self):
        pass

class SimpleConnectionTest(SocketTest):
    def test(self):
        self.connect()
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 220)
        self.close()
        return b

class AuthentificationTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        b = self.authentificate()
        self.close()
        return b

class WrongAuthentificationTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"USER THISOBVIOUSLYISFALSELOL\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 331)
        self.close()
        return b

class SendingBeforeAuthTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        COMMANDS = [
             b"PWD\r\n",
             b"PASV\r\n",
             b"PORT 0,0,0,0,12,34\r\n",
             b"CDUP\r\n",
             b"DELE a\r\n",
             b"LIST\r\n",
             b"RETR a\r\n",
             b"STOR a\r\n",
             b"CWD /\r\n",
        ]

        for command in COMMANDS:
            self.socket.send(command)
            data = self.socket.recv(BUFFER_SIZE)

            b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 530)
            if b == False:
                self.close()
                return False

        self.close()
        return True

class WrongCommandTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"OBVIOUSLYTHISISAFAKECOMMAND\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 500)
        self.close()
        return b

class OnlySpacesTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"                                                                  \r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 500)
        self.close()
        return b

class NoOperationTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"NOOP\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 200)
        self.close()
        return b

class HelpTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"HELP\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        # Timeout is here in case the server continues to yap about his commands which
        # we absolutely do not care about
        try:
            self.socket.settimeout(2)
            self.socket.recv(BUFFER_SIZE)
        except:
            self.close()
            return True

        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 214)
        self.close()
        return b

class MultipleConnectionTest(SocketTest):
    def __init__(self):
        super().__init__()
        self.maxConnections = 30

    def test(self):
        # we don't need it
        self.close()

        for i in range(self.maxConnections):
            t = SimpleConnectionTest()
            if not t.test():
                return False
        return True

class RetrPassiveTest(SocketTest):
    def __init__(self):
        super().__init__()
        self.dataSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def close(self):
        super().close()
        self.dataSocket.close()

    def test(self):
        self.connectAndIgnore()
        if not self.authentificate():
            self.close()
            return False

        self.socket.send(b"PASV\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 227)
        if b == False:
            self.close()
            return False

        pasvPort = Utils.getPasvPort(data.decode("utf-8"))
        if pasvPort is None:
            self.close()
            return False

        self.socket.send(b"RETR tests/wtftp-tests/justafile\r\n")
        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 150)
        if b == False:
            self.close()
            return False

        self.dataSocket.connect((HOST, pasvPort))
        dataSocketData = self.dataSocket.recv(BUFFER_SIZE)
        if dataSocketData != JUSTAFILE_CONTENT.encode():
            self.close()
            return

        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.ending(data) and BufferVerify.statusCode(data, 226)
        self.close()
        return b

class TestWrapper:
    def __init__(self):
        self.passed_tests = 0
        self.failed_tests = 0

    def _executeTest(self, test: SocketTest) -> bool:
        try:
            return test.test()
        except Exception as e:
            print(f"Failed with error {e}")
            return False

    def execute(self, test: SocketTest):
        if self._executeTest(test):
            self.passed_tests += 1
        else:
            self.failed_tests += 1

    def prepare(self):
        with open(os.path.join(BASE_DIR, "wtftp-tests", "justafile"), "w") as f:
            f.write(JUSTAFILE_CONTENT)

        writtenFile = os.path.join(BASE_DIR, "wtftp-tests", "iwrotethisfile")
        if os.path.exists(writtenFile):
            os.remove(writtenFile)

    def test(self):
        self.execute(SimpleConnectionTest())
        self.execute(HelpTest())
        self.execute(MultipleConnectionTest())
        self.execute(AuthentificationTest())
        self.execute(WrongAuthentificationTest())
        self.execute(SendingBeforeAuthTest())
        self.execute(WrongCommandTest())
        self.execute(NoOperationTest())
        self.execute(RetrPassiveTest())

    def display(self):
        print(f"Passed: {self.passed_tests}, Failed: {self.failed_tests}")

def main():
    print("WTFTP: What The File (Transfer Protocol) Testing Program")
    print("--------------------------------------------------------")
    print()

    print("For now, please launch `./myftp 4242 .` from the original my_ftp folder")

    print(f"Connecting to {HOST}:{PORT}")

    wrapper = TestWrapper()
    wrapper.prepare()
    wrapper.test()
    wrapper.display()

if __name__ == "__main__":
    main()
