import socket
import time
from algo import get_move

# Server commends
SET = "SET"
HUM = "HUM"
HME = "HME"
MAP = "MAP"
UPD = "UPD"
END = "END"
BYE = "BYE"

# Client commands
NME = b"NME"
MOV = b"MOV"

def int_to_byte(i): # 0 <= i < 256
    return i.to_bytes(1, "little")

def int_from_byte(b):
    return int.from_bytes(b, "little")

class Client:
    
    """" Map: ( "0": empty, "S": starting point, "Hn": n humans, "Vn": n vampires, "Wn": n werewolves ) """
    
    def __init__(self, name, host, port):
        # Initialize network attributes
        self.name = name
        self.host = host
        self.port = port
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.running = False
        
        # Initialize game attributes
        self.n = 0
        self.m = 0
        self.map = []
        self.species = 'H'
        self.first = False
    
    def connect(self):
        # Connect to the server
        self.client_socket.connect((self.host, self.port))
        self.running = True
        
        # Initialize game
        self.set_name()
    
    def close(self):
        # Close the socket
        self.running = False
        self.client_socket.close()
    
    def __repr__(self):
        repr = "Map:\n"
        for line in self.map:
            for char in line:
                repr += char + '\t'
            repr += '\n'
        return repr
    
    def init_map(self):
        # Build an empty map
        self.map = []
        for i in range(self.n):
            self.map.append(self.m * ["0"])
    
    def map_for_algo(self, method = 0):
        map_algo = [self.n, self.m, self.species, method]
        for y in range(self.n):
            for x in range(self.m):
                if self.map[y][x] != "0":
                    map_algo.append(self.map[y][x])
                    map_algo.append(x)
                    map_algo.append(y)
        return map_algo
    
    def decode_move(self, move):
        print("MOV")
        msg = MOV
        i = 0
        for j in range(1, len(move)):
            if move[j] == 'x':
                n = int(move[i:j])
                msg += int_to_byte(n)
                i = j + 1
                print(n, end = '\t')
        print('\n\n')
        return msg
    
    def handle_data(self, buffer, nbytes):
        bytes_read = 0
        while bytes_read < nbytes:
            # Get command
            cmd = buffer[bytes_read:bytes_read + 3].decode()
            print(cmd)
            bytes_read += 3
            
            # Command SET
            if cmd == SET:
                # Get map dimensions
                self.n = buffer[bytes_read]
                self.m = buffer[bytes_read + 1]
                bytes_read += 2
                print(self.n, end = ' ')
                print(self.m)
                
                # Initialize map
                self.init_map()
            
            # Command HUM
            elif cmd == HUM:
                # Get home number
                n = buffer[bytes_read]
                bytes_read += 1
                print(n)
                
                # For each home
                for i in range(n):
                    # Get position
                    x = buffer[bytes_read]
                    y = buffer[bytes_read + 1]
                    bytes_read += 2
                    print(x, end = '\t')
                    print(y)
                    
                    # Update map
                    self.map[y][x] = "H"
            
            # Command HME
            elif cmd == HME:
                # Get starting position
                x = buffer[bytes_read]
                y = buffer[bytes_read + 1]
                bytes_read += 2
                print(x, end = ' ')
                print(y)
                
                # Update map
                self.map[y][x] = "S"
            
            # Commands MAP
            elif cmd == MAP:
                # Get number of updates
                n = buffer[bytes_read]
                bytes_read += 1
                print(n)
                
                print("x", end = '\t')
                print("y", end = '\t')
                print("h", end = '\t')
                print("v", end = '\t')
                print("w")
                
                # For each update
                for i in range(n):
                    # Get update data
                    x = buffer[bytes_read]
                    y = buffer[bytes_read + 1]
                    h = buffer[bytes_read + 2]
                    v = buffer[bytes_read + 3]
                    w = buffer[bytes_read + 4]
                    bytes_read += 5
                    print(x, end = '\t')
                    print(y, end = '\t')
                    print(h, end = '\t')
                    print(v, end = '\t')
                    print(w)
                    
                    # Update map
                    if h > 0:
                        self.map[y][x] = "H" + str(h)
                    elif v > 0:
                        if self.map[y][x] == "S":
                            self.species = 'V'
                        self.map[y][x] = "V" + str(v)
                    elif w > 0:
                        if self.map[y][x] == "S":
                            self.species = 'W'
                        self.map[y][x] = "W" + str(w)
            
            # Commands UPD
            elif cmd == UPD:
                # Get number of updates
                n = buffer[bytes_read]
                bytes_read += 1
                print(n)
                
                if n > 0:
                    print("x", end = '\t')
                    print("y", end = '\t')
                    print("h", end = '\t')
                    print("v", end = '\t')
                    print("w")
                else:
                    self.first = True
                
                # For each update
                for i in range(n):
                    # Get update data
                    x = buffer[bytes_read]
                    y = buffer[bytes_read + 1]
                    h = buffer[bytes_read + 2]
                    v = buffer[bytes_read + 3]
                    w = buffer[bytes_read + 4]
                    bytes_read += 5
                    print(x, end = '\t')
                    print(y, end = '\t')
                    print(h, end = '\t')
                    print(v, end = '\t')
                    print(w)
                    
                    # Update map
                    if h > 0:
                        self.map[y][x] = "H" + str(h)
                    elif v > 0:
                        self.map[y][x] = "V" + str(v)
                    elif w > 0:
                        self.map[y][x] = "W" + str(w)
                    else:
                        self.map[y][x] = "0"
            
            # Command END
            elif cmd == END:
                # Reset variables
                self.n = 0
                self.m = 0
                self.map = []
                self.species = 'H'
                self.first = False
                
                # Close socket
                self.close()
                
            # Command BYE
            elif cmd == BYE:
                # Close socket
                self.close()
            
            # Print map
            if cmd != END and cmd != BYE:
                print()
                print(self, end = '\n\n')
    
    def set_name(self):
        # Send player's name to the server
        msg = NME + int_to_byte(len(self.name)) + self.name
        self.client_socket.send(msg)
        
        # Initialize game
        self.init_game()
    
    def init_game(self):
        # Receive first two messages to initialize the game map
        buffer = bytearray(1024)
        nbytes = self.client_socket.recv_into(buffer)
        self.handle_data(buffer, nbytes)
        buffer = bytearray(1024)
        nbytes = self.client_socket.recv_into(buffer)
        self.handle_data(buffer, nbytes)
        
        # Start the game
        self.start_game()
    
    def start_game(self):
        # Play the game
        while self.running:
            # Receive data from server to update map
            buffer = bytearray(1024)
            nbytes = self.client_socket.recv_into(buffer)
            self.handle_data(buffer, nbytes)
            if not self.running:
                break
            
            # Get next move from C++ algo
            map_algo = self.map_for_algo()
            move = get_move(map_algo)
            msg = self.decode_move(move)
            
            # Send player's move to server
            self.client_socket.send(msg)

# Create and connect a client to game server
client = Client(b"Towelie33", "localhost", 5555)
client.connect()
