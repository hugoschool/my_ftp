import os
import socket
import time
import random
import threading
import sys
import signal

BASE_DIR = os.path.dirname(os.path.realpath(__file__))

TESTS_DIRNAME = "wtftp-tests"
TESTS_DIR = os.path.join(BASE_DIR, TESTS_DIRNAME)

MYFTP_DIR = os.path.join(BASE_DIR, "..")

HOST = "127.0.0.1"
PORT = random.randint(4000, 6000)

CRLF = b"\r\n"
BUFFER_SIZE = 1024

JUSTAFILE_CONTENT = "Hello this is just a test for WTFTP!"
IWROTETHISFILE_PATH = os.path.join(TESTS_DIR, "iwrotethisfile")
IWROTETHISFILE_CONTENT = "Yeah I just wrote this and it's pretty\ngood lol!"
LIST_FILES = [
    b"file1",
    b"file2",
    b"file3",
    b"file4",
    b"anotherdirectory",
]

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    PURPLE = '\033[35m'
    RESET = '\033[0m'

class BufferVerify:
    @staticmethod
    def statusCode(buffer, statusCode: int):
        return_value = BufferVerify.ending(buffer) and buffer.startswith(str(statusCode).encode())

        if return_value is False:
            print()
            print("Buffer does not have the right status code")
            print(f"    Given: {buffer}")
            print(f"    Expected status code: {statusCode}")

        return return_value

    @staticmethod
    def ending(buffer) -> bool:
        return_value = buffer.endswith(CRLF)

        if return_value is False:
            print("Buffer does not end with CRLF")

        return return_value

class Utils:
    @staticmethod
    def getPasvPort(buffer) -> int | None:
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
        self.socket.settimeout(15)

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

        if not BufferVerify.statusCode(data, 331):
            b = False
            return b

        self.socket.send(b"PASS\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.statusCode(data, 230)
        return b

    def test(self):
        pass

class SimpleConnectionTest(SocketTest):
    def test(self):
        self.connect()
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.statusCode(data, 220)
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

        b = BufferVerify.statusCode(data, 331)
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

            b = BufferVerify.statusCode(data, 530)
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

        b = BufferVerify.statusCode(data, 500)
        self.close()
        return b

class OnlySpacesTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"                                                                  \r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.statusCode(data, 500)
        self.close()
        return b

class NoOperationTest(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"NOOP\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.statusCode(data, 200)
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

        b = BufferVerify.statusCode(data, 214)
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

class DataSocketTest(SocketTest):
    def __init__(self):
        super().__init__()
        self.dataSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.dataSocket.settimeout(30)

    def close(self):
        super().close()
        self.dataSocket.close()

    def passiveConnection(self) -> int | None:
        self.connectAndIgnore()
        if not self.authentificate():
            self.close()
            return None

        self.socket.send(b"PASV\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.statusCode(data, 227)
        if b == False:
            return None

        pasvPort = Utils.getPasvPort(data.decode("utf-8"))
        return pasvPort

    def activeConnection(self) -> bool:
        self.connectAndIgnore()
        if not self.authentificate():
            self.close()
            return False

        dataPort = 38592
        self.dataSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.dataSocket.bind((HOST, dataPort))
        self.dataSocket.listen(2)

        self.socket.send(b"PORT 0,0,0,0,150,192\r\n")
        data = self.socket.recv(BUFFER_SIZE)

        b = BufferVerify.statusCode(data, 200)
        if b == False:
            self.close()
            return False

        return True

    def activeAccept(self) -> socket:
        self.dataSocket.settimeout(5) # Max 5 second timeout for the accept
        acceptedSocket = self.dataSocket.accept()
        return acceptedSocket[0]

class RetrPassiveTest(DataSocketTest):
    def test(self):
        dataPort = self.passiveConnection()
        if not dataPort:
            self.close()
            return False

        self.socket.send(f"RETR {TESTS_DIRNAME}/justafile\r\n".encode())
        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 150)
        if b == False:
            self.close()
            return False

        self.dataSocket.connect((HOST, dataPort))
        dataSocketData = self.dataSocket.recv(BUFFER_SIZE)
        if dataSocketData != JUSTAFILE_CONTENT.encode():
            self.close()
            return

        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 226)
        self.close()
        return b

class RetrActiveTest(DataSocketTest):
    def test(self):
        if not self.activeConnection():
            self.close()
            return False

        self.socket.send(f"RETR {TESTS_DIRNAME}/justafile\r\n".encode())
        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 150)
        if b == False:
            self.close()
            return False

        dataSocketData = self.activeAccept().recv(BUFFER_SIZE)
        if dataSocketData != JUSTAFILE_CONTENT.encode():
            self.close()
            return

        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 226)
        self.close()
        return b

class ListPassiveTest(DataSocketTest):
    def test(self):
        dataPort = self.passiveConnection()
        if not dataPort:
            self.close()
            return False

        self.socket.send(f"CWD {TESTS_DIRNAME}/something\r\n".encode())
        self.socket.recv(BUFFER_SIZE)

        self.socket.send(b"LIST\r\n")
        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 150)
        if b == False:
            self.close()
            return False

        self.dataSocket.connect((HOST, dataPort))

        fullData = b""
        dataSocketData = self.dataSocket.recv(BUFFER_SIZE)
        while len(dataSocketData) > 0:
            fullData += dataSocketData
            dataSocketData = self.dataSocket.recv(BUFFER_SIZE)

        for file in LIST_FILES:
            if file not in fullData:
                self.close()
                return False

        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 226)
        self.close()
        return b

class ListActiveTest(DataSocketTest):
    def test(self):
        if not self.activeConnection():
            self.close()
            return False

        self.socket.send(f"CWD {TESTS_DIRNAME}/something\r\n".encode())
        self.socket.recv(BUFFER_SIZE)

        self.socket.send(b"LIST\r\n")
        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 150)
        if b == False:
            self.close()
            return False

        acceptedSocket = self.activeAccept()
        fullData = b""
        dataSocketData = acceptedSocket.recv(BUFFER_SIZE)
        while len(dataSocketData) > 0:
            fullData += dataSocketData
            dataSocketData = acceptedSocket.recv(BUFFER_SIZE)

        for file in LIST_FILES:
            if file not in fullData:
                self.close()
                return False

        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 226)
        self.close()
        return b

class StorTests(DataSocketTest):
    def close(self):
        super().close()

    def _testWrapper(self):
        pass

    def test(self):
        if self._testWrapper() == False:
            return False

        try:
            with open(IWROTETHISFILE_PATH, "r") as f:
                content = f.read()
                os.remove(IWROTETHISFILE_PATH)
                return content == IWROTETHISFILE_CONTENT
            return False
        except:
            return False

class StorActiveTest(StorTests):
    def _testWrapper(self):
        if not self.activeConnection():
            self.close()
            return False

        self.socket.send(f"STOR {TESTS_DIRNAME}/iwrotethisfile\r\n".encode())
        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 150)
        if b == False:
            self.close()
            return False

        acceptedSocket = self.activeAccept()
        dataSocketData = acceptedSocket.send(IWROTETHISFILE_CONTENT.encode())
        acceptedSocket.close()

        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 226)
        self.close()
        return b

class StorPassiveTest(StorTests):
    def _testWrapper(self):
        dataPort = self.passiveConnection()
        if not dataPort:
            self.close()
            return False

        self.socket.send(f"STOR {TESTS_DIRNAME}/iwrotethisfile\r\n".encode())
        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 150)
        if b == False:
            self.close()
            return False

        self.dataSocket.connect((HOST, dataPort))
        dataSocketData = self.dataSocket.send(IWROTETHISFILE_CONTENT.encode())
        self.dataSocket.close()

        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 226)
        self.close()
        return b

class HalfCommandOne(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"USER Ano")

        time.sleep(2)

        self.socket.send(b"nymous\r\n")

        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 331)
        self.close()
        return b

class HalfCommandTwo(SocketTest):
    def test(self):
        self.connectAndIgnore()

        self.socket.send(b"USER Anonymous\r\nP")
        data = self.socket.recv(BUFFER_SIZE)
        if not  BufferVerify.statusCode(data, 331):
            self.close()
            return False

        self.socket.send(b"ASS\r\n")
        data = self.socket.recv(BUFFER_SIZE)
        b = BufferVerify.statusCode(data, 230)
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

    def execute(self, test: SocketTest, name: str = None):
        if self._executeTest(test):
            self.passed_tests += 1
            print(f"{Colors.GREEN}{name} passed{Colors.RESET}")
        else:
            print(f"{Colors.RED}{name} failed{Colors.RESET}")
            print()
            self.failed_tests += 1

    def prepare(self):
        if not os.path.exists(TESTS_DIR):
            os.mkdir(TESTS_DIR)

        with open(os.path.join(TESTS_DIR, "justafile"), "w") as f:
            f.write(JUSTAFILE_CONTENT)

        # For the LIST test
        DIRECTORY = os.path.join(TESTS_DIR, "something")
        if not os.path.exists(DIRECTORY):
            os.mkdir(DIRECTORY)
            files = [
                os.path.join(DIRECTORY, "file1"),
                os.path.join(DIRECTORY, "file2"),
                os.path.join(DIRECTORY, "file3"),
                os.path.join(DIRECTORY, "file4"),
            ]
            for file in files:
                with open(file, "w") as f:
                    f.write("a")
            os.mkdir(os.path.join(DIRECTORY, "anotherdirectory"))

        # For the STOR test
        writtenFile = os.path.join(TESTS_DIR, "iwrotethisfile")
        if os.path.exists(writtenFile):
            os.remove(writtenFile)

    def test(self):
        self.execute(SimpleConnectionTest(), "SimpleConnectionTest")
        self.execute(HelpTest(), "HelpTest")
        self.execute(MultipleConnectionTest(), "MultipleConnectionTest")
        self.execute(AuthentificationTest(), "AuthentificationTest")
        self.execute(WrongAuthentificationTest(), "WrongAuthentificationTest")
        self.execute(SendingBeforeAuthTest(), "SendingBeforeAuthTest")
        self.execute(WrongCommandTest(), "WrongCommandTest")
        self.execute(NoOperationTest(), "NoOperationTest")
        self.execute(RetrPassiveTest(), "RetrPassiveTest")
        self.execute(RetrActiveTest(), "RetrActiveTest")
        self.execute(ListPassiveTest(), "ListPassiveTest")
        self.execute(ListActiveTest(), "ListActiveTest")
        self.execute(StorActiveTest(), "StorActiveTest")
        self.execute(StorPassiveTest(), "StorPassiveTest")
        self.execute(HalfCommandOne(), "HalfCommandOne")
        self.execute(HalfCommandTwo(), "HalfCommandTwo")
        # TODO: Deleting files
        # TODO: Working directory

    def display(self):
        print(f"Passed: {Colors.GREEN}{self.passed_tests}{Colors.RESET}, Failed: {Colors.RED}{self.failed_tests}{Colors.RESET}")

class myFTP:
    def __init__(self, file: str):
        self.file = file
        self.pid = -1

        if not os.access(self.file, os.X_OK):
            raise Exception("File is not executable")

    def run(self):
        try:
            self.pid = os.fork()
        except:
            print("Couldn't fork to launch myftp, closing.")
            sys.exit(1)

        if self.pid == 0:
            # Redirect all outputs to /dev/null
            fd = os.open("/dev/null", os.O_WRONLY)
            os.dup2(fd, 1)
            os.dup2(fd, 2)
            os.execv(self.file, [self.file, str(PORT), BASE_DIR])
        else:
            # Wait for server to be up to run
            time.sleep(3)

    def close(self):
        if self.pid != -1:
            try:
                os.kill(self.pid, signal.SIGTERM)
            except OSError:
                print(f"Had a problem closing {self.file}")
                sys.exit(1)

def main():
    if len(sys.argv) == 3 and sys.argv[1] == "-c":
        MYFTP_FILE = os.path.realpath(sys.argv[2])
    else:
        MYFTP_FILE = os.path.join(MYFTP_DIR, "myftp")

    print(Colors.PURPLE, end="")
    print("WTFTP: What The File (Transfer Protocol) Testing Program")
    print("--------------------------------------------------------")
    print(Colors.RESET, end="")
    print()

    myftp_bin = myFTP(MYFTP_FILE)
    myftp_bin.run()

    print(f"Connecting to {HOST}:{PORT}")
    print()

    wrapper = TestWrapper()
    wrapper.prepare()
    wrapper.test()
    wrapper.display()

    myftp_bin.close()

if __name__ == "__main__":
    main()
