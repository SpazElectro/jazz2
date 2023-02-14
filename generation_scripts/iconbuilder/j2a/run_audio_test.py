
from __future__ import print_function
import os
import sys
import struct
from types import FunctionType

from j2a import J2A
import misc

if sys.version_info[0] <= 2:
    input = raw_input

def _read_hdr():
    global anims, anims_path
    if "anims" in globals():
        return anims
    else:
        print("Reading animations file", anims_path)
        return J2A(anims_path).read()

class RIFFChunk(object):
    _Header = struct.Struct("<4sL")

    def __init__(self, name, data, slack = None, staticdata=b''):
        self.name = name
        self.staticdata = staticdata
        if slack is None:
            self.subchunks, self.raw = RIFFChunk.collector(data)
        else:
            self.subchunks, self.raw = data, slack

    @staticmethod
    def collector(data, namesize=4, staticdatasize=0, inclusive_sizes=False):
        debug_print = print if staticdatasize else lambda *pargs, **kwargs : None
        assert isinstance(data, bytes)
        chunks = []
        Header = struct.Struct("<%dsL%ds" % (namesize, staticdatasize))
        debug_print("# Launching collector with header of size", Header.size)
        while len(data) >= Header.size:
            debug_print("# Iteration on data", data[:16], "...")
            name, size, staticdata = Header.unpack_from(data, 0)
            debug_print("# 1)", name)
            if not all(c in b' ABCDEFGHIJKLMNOPQRSTUVWXYZ' for c in name):
                debug_print("# Bailing on step 1")
                break
            debug_print("# 2)", size, "+", Header.size, "<=", len(data))
            cut_offset = size + (0 if inclusive_sizes else Header.size - staticdatasize)
            if cut_offset > len(data):
                debug_print("# Bailing on step 2")
                break
            chunks.append(RIFFChunk(name, [], data[Header.size:cut_offset], staticdata=staticdata))
            data = data[cut_offset:]
            debug_print("# 3)", len(data), ">=", Header.size)
        return (chunks, data)

    @staticmethod
    def chunkify_buffer(data):
        return RIFFChunk.collector(data, namesize=0, inclusive_sizes=True)

def test_RIFF(filename):
    with open("temp/set0sampledata.raw", "rb") as f:
        samples_data = f.read()
    mainchunk = RIFFChunk(b'MAIN', *RIFFChunk.chunkify_buffer(samples_data))
    assert len(mainchunk.subchunks) > 0 and len(mainchunk.raw) == 0
    for i,c in enumerate(mainchunk.subchunks):
        c.subchunks, c.raw = RIFFChunk.collector(c.raw, 4, 4, False)
        assert len(c.subchunks) == 1 and len(c.raw) == 0
        riff_c = c.subchunks[0]
        assert riff_c.name == b'RIFF'
        riff_c.subchunks, riff_c.raw = RIFFChunk.collector(riff_c.raw)

    def dump_chunks(chunk, indent=0):
        print("{0}Chunk '{1}': {2} subchunks, sdata {3}, slack of length {4}; slack: {5}"
            .format("\t" * indent, chunk.name.decode(), len(chunk.subchunks), chunk.staticdata, hex(len(chunk.raw)), chunk.raw))
        for c in chunk.subchunks:
            dump_chunks(c, indent + 1)

    dump_chunks(mainchunk)

S_Header = misc.NamedStruct(
    "L|total_size/"      # 0x0  - 0x4  (inclusive)
    "4s|riff_id/"        # 0x4  - 0x8  "RIFF"
    "L|riff_size/"       # 0x8  - 0xC  total_size - 0xc (exclusive)
    "4s|format/"         # 0xC  - 0x10 "AS  "
    "4s|sc_id/"          # 0x10 - 0x14 "SAMP"
    "L|sc_size/"         # 0x14 - 0x18 total_size - 0x18-19 (exclusive)
    "L|reserved1_size/"  # 0x18 - 0x1C = 0x40
    "32s|reserved1/"     # 0x1C - 0x3C zeros
    "H|unknown1/"        # 0x3C - 0x3E = 0x4000
    "h|volume/"          # 0x3E - 0x40 0 - 0x7fff
    "H|flags/"           # 0x40 - 0x42 0x4 => 16-bit LE samples; 0x8 => loop; 0x10 => BiDi loop; 0x40 => stereo
    "H|unknown2/"        # 0x42 - 0x44 = 0x0080
    "L|nsamples/"        # 0x44 - 0x48
    "L|loop_start/"      # 0x48 - 0x4C
    "L|loop_end/"        # 0x4C - 0x50
    "L|sample_rate/"     # 0x50 - 0x54
    "L|has_appendix/"    # 0x54 - 0x58 if != 0, 0x9e additional bytes in header
    "L|reserved2"        # 0x58 - 0x5C
)
def dump_samples(folder):
    import os
    import wave

    anims = _read_hdr().unpack()
    if not os.path.exists(folder):
        os.makedirs(folder)

    for set_num,s in enumerate(anims.sets):
        set_folder = "{0}/{1:03}".format(folder, set_num)
        if not os.path.exists(set_folder):
            os.mkdir(set_folder)
        for sample_num,sample in enumerate(s.samples):
            with wave.open("{0}/{1:03}.wav".format(set_folder, sample_num), "wb") as w:
                w.setparams((sample._channels, sample._channels * sample._bits // 8, sample._rate, len(sample._data), "NONE", "not compressed"))
                w.writeframes(bytes(b ^ 0x80 for b in sample._data))

class PyAudioSoundPlayer(object):
    def __init__(self):
        import pyaudio
        self._p = pyaudio.PyAudio()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._p.terminate()
        return False

    def play(self, samples, format, nchannels, framerate, verbose=True, **kwargs):
        import pyaudio
        if kwargs:
            print("Warning: ignored extra keyword arguments:", *kwargs)
        if verbose:
            print("Using sampling rate:", framerate)
        sample_type = getattr(pyaudio, "pa" + format)
        stream = self._p.open(format=sample_type,
                              channels=nchannels,
                              rate=framerate,
                              output=True)
        if isinstance(samples, (bytes, bytearray)):
            samples = (samples, )
        for s in samples:
            stream.write(s)
        stream.write(b'\x00' * 8000)  # Some silence, to avoid early cutting off
        stream.stop_stream()

def _play_sample(audio_out, anims, set_num, sample_num, sample_rate):
    s = anims.sets[int(set_num)]

    if isinstance(sample_rate, str):
        if sample_rate[:1] == '*':
            sample_rate = float(sample_rate[1:])
        else:
            sample_rate = int(sample_rate)

    if isinstance(sample_rate, int):
        get_rate = lambda rate : sample_rate
    else:
        get_rate = lambda rate : int(rate * sample_rate)

    if sample_num != "*":  # Expects an int-like
        sample_num = int(sample_num)
        sample_list = (sample_num, )
    else:
        sample_list = range(len(s.samples))

    for i in sample_list:
        print("Set %d, sample %d" % (int(set_num), i))
        sample = s.samples[i]
        is_16bit  = bool(sample._bits == 16)
        rate = sample._rate
        print("Reported sampling rate:", rate)
        audio_out.play(sample._data, nchannels = sample._channels, framerate = get_rate(rate), format = ("Int8", "Int16")[is_16bit])

def play_sample(set_num, sample_num, sample_rate=1.0):
    '''
    Play a sample from Anims.j2a

    `set_num` and `sample_num` specify the set and sample number
    `sample_rate` can be either:
     - an integer, specifying the sample rate directly;
     - a string of the type "*#", where # is a floating point number, in which case
       the sample rate used is the reported sample rate * #
     - omitted, which is equivalent to "*1"
    '''

    with PyAudioSoundPlayer() as audio_out:
        _play_sample(audio_out, _read_hdr(), set_num, sample_num, sample_rate)

def sample_console():
    '''
    A minimal console to play with samples from Anims.j2a
    Commands:
        <x> <y>         Play sample y from set x, at its nominal framerate
        <x> <y> 22050   Play sample y from set x, at framerate 22050
        <x> <y> *0.5    Play sample y from set x, at half its nominal framerate
        <x> *           Play all samples from set x, at their nominal framerate
        <x> * *2        Play sample y from set x, at twice their nominal framerate
        ... and so on
    '''
    anims = _read_hdr().unpack()
    s_old = ""
    print("Type CTRL-C to exit")

    with PyAudioSoundPlayer() as audio_out:
        while True:
            try:
                s = input("> ")
                s = s or s_old
                s_old = s

                l = s.split()
                assert len(l) <= 3
                set_num, sample_num = l[:2]
                sample_rate = ( l[2:3] or (1.0,) )[0]
                _play_sample(audio_out, anims, set_num, sample_num, sample_rate)
            except EOFError:
                print()
                return
            except IndexError:
                print("Value out of bounds")
                continue
            except OSError as e:
                print("AudioOut threw an exception: OSError:", e)
                continue
            except Exception as e:
                print("Invalid command line:", s)
                print("The following exception was thrown:", str(type(e)) + ":", e)
                continue

def dump_samples_data_slice(filename, start=0, size=0x20):
    anims = _read_hdr().unpack()
    start = start if isinstance(start, int) else int(start, 0)
    size  = size  if isinstance(size,  int) else int(size,  0)
    nsegs = (size - 1) // 4 + 1
    fmt_str = "{0:3} {1:3} -" + (" {%d}" * nsegs) % tuple(range(2, nsegs+2))
    interv = (start, start + 4*nsegs, 4)
    to_hex = lambda x : ("%02x" * len(x)) % tuple(x)
    with open(filename, "w") as f:
        print("        -" + (" %-8x" * nsegs) % tuple(range(*interv)), file=f)
        for set_num,s in enumerate(anims.sets):
            for sample_num,raw in enumerate(s.samples):
                raw = bytearray(raw)
                print(fmt_str.format(
                    set_num, sample_num,
                    *(to_hex(raw[i:i+4]) for i in range(*interv))
                ), file=f)

#############################################################################################################

if __name__ == "__main__":
    fmap = dict((k, v) for k,v in globals().items() if isinstance(v, FunctionType) and not k.startswith('_'))

    assert int(True) is 1
    isint = lambda x : x[int(x[:1] in '+-'):].isdigit()

    anims_path = None
    fargs = []
    for arg in sys.argv[2:]:
        if arg.endswith('.j2a'):
            anims_path = arg
        else:
            if isint(arg): # Don't use integers for file names
                arg = int(arg)
            fargs.append(arg)
    anims_path = anims_path or os.path.join(os.path.dirname(sys.argv[0]), "Anims.j2a")

    print("Calling {0} with arguments: {1}".format(sys.argv[1], fargs))
    retval = fmap[sys.argv[1]](*fargs)
    if isinstance(retval, int):
        sys.exit(retval)
