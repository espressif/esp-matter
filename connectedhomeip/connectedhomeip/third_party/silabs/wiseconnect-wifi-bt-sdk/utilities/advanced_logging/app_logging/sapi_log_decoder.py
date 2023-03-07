from json import load as jload
from sys import argv

# for testing only
def tostrbits(data):
    ret = ""
    for byte in data:
        print(byte)
        ret += bin(byte)[2:].zfill(8)
        # ret += bin(int.from_bytes(byte, 'big'))[2:]
    return ret


def b2i(byte):
    return int.from_bytes(byte, "little")


class Decoder:
    """Decoder class"""

    def __init__(self):
        # head indicates current position of state machine
        self.head = 0
        # cumulative tsf
        self.cum_tsf = 0

    def _hex_to_byte(self, s):
        """Convert hex(ASCII) to bytes"""
        return bytes([int(s, 16)])

    def load_file_json(self, path):
        """Load a JSON file (manifest)"""
        with open(path) as file:
            self.json = jload(file)

    def load_file_bin(self, path):
        """Load binary debug data"""
        with open(path, "rb") as file:
            self.data = file.read()
            self.length = 8 * len(self.data)

    def load_file_hex(self, path):
        """Load hexadecimal debug data"""
        with open(path, "r") as file:
            self.data = list(map(self._hex_to_byte, file.read().split()))
            self.length = 8 * len(self.data)

    def load_file_ascii(self, path):
        """Load possibly polluted ASCII debug data"""
        data = []
        with open(path) as file:
            for line in file.readlines():
                spline = line.split()
                if spline and spline[0] == "#@$":
                    if spline[-1][0] == '<':
                        spline.pop()
                    data.extend(spline[1:])
        self.data = list(map(self._hex_to_byte, data))
        self.length = 8 * len(self.data)

    def get(self, n):
        """Get the next n bits from self.data"""

        h = self.head
        bh = h // 8
        ret = 0
        if h + n > self.length:
            raise ValueError("get value too large.")
        copy_n = n
        while n:
            bufl = 8 - (h % 8)
            chunk = int.from_bytes(self.data[bh], "little") & ((1 << bufl) - 1)
            if n < bufl:
                chunk >>= bufl - n
                ret = (ret << n) | chunk
                n = 0
            else:
                ret = (ret << bufl) | chunk
                n -= bufl
            bh += 1
            h = 0
        self.head += copy_n
        return ret

    def _print_remaining(self):
        """
        Prints remaining contents of self.data
        For debugging only
        """

        for byte in self.data[self.head // 8 :]:
            print(hex(b2i(byte))[2:].zfill(2), end=" ")
        print()

    def get_log_version(self):
        """Get the log version of manifest file"""

        return self.json["SL_LOG_VERSION"]

    def data_to_be_read(self):
        """Return true if there is data to be read"""
        return self.head < self.length

    def read_next(self):
        """Read and parse the next "packet" of data"""
        SINGAL_LOST_SIZE = 1
        TSF_PRESENT_SIZE = 1
        DID_FORMAT_SIZE = 1
        LDID_SIZE = 5
        HDID_SIZE = 7 + 1
        TSF_FORMAT_SIZE = 1
        LTSF_SIZE = 7
        HTSF_SIZE = 8

        BYTE = 0b00
        WORD = 0b01
        STRING = 0b10
        DWORD = 0b11

        ldid = self.get(LDID_SIZE)
        did_format = self.get(DID_FORMAT_SIZE)
        tsf_present = self.get(TSF_PRESENT_SIZE)
        signal_lost = self.get(SINGAL_LOST_SIZE)

        # print("TSF_P:", tsf_present)
        # print("DIDF:", did_format)
        # print("LDID:", ldid)

        hdid = 0
        if did_format:
            hdid = self.get(HDID_SIZE)
        did = hdid << LDID_SIZE | ldid
        tsf = 0
        if tsf_present:
            ltsf = self.get(LTSF_SIZE)
            tsf_format = self.get(TSF_FORMAT_SIZE)
            htsf = 0
            if tsf_format:
                htsf = self.get(HTSF_SIZE)
            tsf = htsf << LDID_SIZE | ltsf
        self.cum_tsf += tsf

        did = str(did)
        # print("DID:", did)
        # print("TSF:", tsf)

        if signal_lost:
            self._print_remaining()
            raise RuntimeError("Signal lost.")

        if did not in self.json:
            raise KeyError("DebugID missing in JSON.")

        info = self.json[str(did)]
        parsee = info["format"]
        num_args = info["argnum"]

        # print("FORMAT STRING:", parsee)
        # print("NUM ARGS:", num_args)
        # print()

        for argpos in range(num_args):
            idx = parsee.find("%")
            sub = parsee[idx : idx + 2]
            if sub[-1].isnumeric():
                sub += parsee[idx + 2]

            argtype = int(info["arg" + str(argpos + 1) + "type"])

            # print("ARGTYPE:", argtype)
            # print("SUBSTRING:", sub)

            # string
            if argtype is STRING and sub == "%s":
                data = str()
                while True:
                    next_byte = self.get(8)
                    if not next_byte:
                        break
                    data += chr(next_byte)
                parsee = parsee.replace("%s", data, 1)

            elif argtype is BYTE and sub in ("%b", "%c", "%1x"):
                if sub == "%b":
                    data = self.get(8)
                    parsee = parsee.replace("%b", str(data), 1)
                elif sub == "%c":
                    data = self.get(8)
                    parsee = parsee.replace("%c", chr(data), 1)
                elif sub == "%1x":
                    data = self.get(8)
                    parsee = parsee.replace("%1x", hex(data), 1)

            elif argtype is WORD and sub == "%2x":
                data = 0
                for shift in range(2):
                    data |= self.get(8) << (8*shift)
                parsee = parsee.replace("%2x", hex(data), 1)

            elif argtype is DWORD and sub in ("%4x", "%d"):
                if sub == "%d":
                    data = 0
                    for shift in range(4):
                        data |= self.get(8) << (8*shift)
                    parsee = parsee.replace("%d", str(data), 1)
                elif sub == "%4x":
                    data = 0
                    for shift in range(4):
                        data |= self.get(8) << (8*shift)
                    parsee = parsee.replace("%4x", hex(data), 1)

            else:
                raise RuntimeError("Format mismatch.")

        return str(self.cum_tsf).zfill(6) + " " + info["debug_id"] + "(" + parsee + ")"


def main():

    argc = len(argv)
    if argc != 2:
        print("Usage: python log_decoder.py path/to/dbgdata.txt")
        exit(0)
    decoder = Decoder()
    decoder.load_file_json("manifest.json")
    decoder.load_file_ascii(argv[1])
    print("SL_LOG_VERSION:", decoder.get_log_version())
    while decoder.data_to_be_read():
        print(decoder.read_next())


if __name__ == "__main__":
    main()
