
from __future__ import print_function
import os
import sys
import struct
import itertools
import zlib
from types import FunctionType

from j2a import J2A

if sys.version_info[0] <= 2:
    input = raw_input
    if isinstance(__builtins__, dict):  # Needed for cProfile with Python 2
        class BuiltinsWrapper(object):
            __getattr__ = __builtins__.__getitem__
            __setattr__ = __builtins__.__setitem__
        builtins = BuiltinsWrapper()
    else:
        builtins = __builtins__
else:
    import builtins

def _read_hdr():
    global anims, anims_path
    if "anims" in globals():
        return anims
    else:
        print("Reading animations file", anims_path)
        return J2A(anims_path).read()

_original_open = open

def _open_mock(filename, mode):
    import io
    if filename.startswith("TEST"):
        assert mode in ("rb", "wb")
        if mode == "rb":
            retval = _setup_pseudo_io.outputs[filename]
            retval.seek(0)
        else:  # mode == "wb"
            retval = io.BytesIO()
            retval.close = lambda : None  # Closing a BytesIO object destroys the buffer
            _setup_pseudo_io.outputs[filename] = retval
        return retval
    else:
        return _original_open(filename, mode)

def _setup_pseudo_io(restore=False):
    if restore:
        builtins.open = _original_open
    else:
        builtins.open = _open_mock
_setup_pseudo_io.outputs = {}

def _assert_equal_anims(anims1, anims2):
    assert isinstance(anims1, J2A)
    assert isinstance(anims2, J2A)
    assert len(anims1.sets) == len(anims2.sets)
    for set1, set2 in zip(anims1.sets, anims2.sets):
        assert len(set1.animations) == len(set2.animations)
        assert len(set1.samples)    == len(set2.samples)
        for anim1, anim2 in zip(set1.animations, set2.animations):
            assert anim1.fps         == anim2.fps
            assert len(anim1.frames) == len(anim2.frames)
            for frame1, frame2 in zip(anim1.frames, anim2.frames):
                frame1.encode_image(), frame2.encode_image()
                for fprop in ("shape", "origin", "coldspot", "gunspot", "_rle_encoded_pixmap", "tagged"):
                    assert getattr(frame1, fprop) == getattr(frame2, fprop)
                if frame1.mask != frame2.mask:
                    for mask in (frame1.mask, frame2.mask):
                        assert mask is None or not any(mask)
        for samp1, samp2 in zip(set1.samples, set2.samples):
            for sprop in ("_data", "_rate", "volume", "_bits", "_channels", "loop"):
                assert getattr(samp1, sprop) == getattr(samp2, sprop)
#                         assert pixmap is None or not any(any(row) for row in pixmap)

#############################################################################################################

def show_frame(set_num, anim_num, frame_num):
    try:
        import matplotlib.pyplot as plt
        def show_img(img):
            plt.imshow(img)
            plt.axis('off')
            plt.show()
    except ImportError:
        def show_img(img):
            img.show()

    anims = _read_hdr()
    try:
        frame = anims.sets[set_num].animations[anim_num].frames[frame_num]
    except IndexError:
        print("Error: some index was out of bounds")
        return 1

    rendered = anims.render_pixelmap(frame)
    show_img(rendered)

def show_anim(set_num, anim_num):
    import numpy as np
    import matplotlib.pyplot as plt
    from matplotlib.animation import ArtistAnimation

    anims = _read_hdr()
    s = anims.sets[set_num]
    anim = s.animations[anim_num]

    images = [anims.render_pixelmap(frame) for frame in anim.frames]
    fps = anim.fps

    borders = np.array([[x, x + w, -y - h, -y] for frame in anim.frames for x, y, w, h in [frame.origin + frame.shape]])
    extremes = ((borders[:,0].min(), borders[:,1].max()), (borders[:,2].min(), borders[:,3].max()))

    fig, ax = plt.subplots()
    artists = [[plt.imshow(image, animated=True, extent=borders[i])] for i,image in enumerate(images)]
    ani = ArtistAnimation(fig, artists, interval=1000.0/fps, blit=True)
    ax.axis("off")
    ax.set_xlim(extremes[0])
    ax.set_ylim(extremes[1])
    plt.show()

def print_j2a_stats():
    anims = _read_hdr()
    print("Jazz Jackrabbit 2 animations file")
    print("\tsetcount: {0}".format(len(anims.sets)))
    for i,s in enumerate(anims.sets):
        print("\tSet {0}:".format(i))
        print("\t\tanimcount: {0}".format(len(s.animations)))
        print("\t\tsamplecount: {0}".format(s._samplecount))
        print("\t\tframecount: {0}".format(sum(len(a.frames) for a in s.animations)))

def generate_compmethod_stats(filename, starting_set=0):
    l_level = list(range(1, 10))
    l_method = [zlib.DEFLATED]
    l_wbits = [15]
    l_memLevel = list(range(1, 10))
    l_strategy = list(range(0, 4))  # 4 (= Z_FIXED) causes SEGFAULT sometimes

    anims = _read_hdr()
    struct = generate_compmethod_stats.struct

    def dump(f, raw, setnum, chknum, level, method, wbits, memLevel, strategy):
        print(setnum, chknum, pargs)
        cobj = zlib.compressobj(*pargs)
        length = len(cobj.compress(raw)) + len(cobj.flush())
        f.write(struct.pack(setnum, chknum, level, method, wbits, memLevel, strategy, length))

    with open(filename, "wb") as f:
        for setnum, s in enumerate(anims.sets):
            if setnum < starting_set:
                continue
            print("Dumping for set", setnum)
            for chknum, chk in enumerate(s._chunks):
                raw = zlib.decompress(chk[0], zlib.MAX_WBITS, chk[1])
                [dump(f, raw, setnum, chknum, level, method, wbits, memLevel, strategy)
                    for level    in l_level
                    for method   in l_method
                    for wbits    in l_wbits
                    for memLevel in l_memLevel
                    for strategy in l_strategy
                ]
generate_compmethod_stats.struct = struct.Struct("<BBBBBBBL")

def stress_test():
    anims = _read_hdr()
    for s in anims.sets:
        for anim in s.animations:
            for frame in anim.frames:
                anims.render_pixelmap(frame)

def writing_test():
    anims = _read_hdr()
    anims.unpack()
    _setup_pseudo_io()
    anims.write("TEST")
    anims2 = J2A("TEST").read().unpack()

def unpacking_test():
    anims = _read_hdr()
    anims.unpack()

def repacking_test():
    anims = _read_hdr()
    pristine_chunks = [s._chunks for s in anims.sets]
    anims.unpack().pack()
#     pristine_chunks[-1][2] = (zlib.compress(b'\x00'), 0)
    new_chunks = [s._chunks for s in anims.sets]
    failed = False
    for i, (pset, nset) in enumerate(zip(pristine_chunks, new_chunks)):
        for chunk_num, pchunk, nchunk in zip(range(4), pset, nset):
            if zlib.decompress(pchunk[0], zlib.MAX_WBITS, pchunk[1]) != zlib.decompress(nchunk[0], zlib.MAX_WBITS, nchunk[1]):
                print("Difference in set %d, chunk %d" % (i, chunk_num))
                failed = True
    print("Packing test", "FAILED" if failed else "PASSED")

def full_cycle():
    anims = _read_hdr()
    for s in anims.sets:
        for anim in s.animations:
            for frame in anim.frames:
                frame.decode_image()
    _setup_pseudo_io()
    anims.write("TEST")

def full_cycle_test():
    anims1 = _read_hdr()
    for s in anims1.sets:
        for anim in s.animations:
            for frame in anim.frames:
                frame.decode_image()
    _setup_pseudo_io()
    anims1.write("TEST")
    anims2 = J2A("TEST").read()
    _assert_equal_anims(anims1, anims2)
    print("Test PASSED")

def _random_pixmap(seed=None, width=260, height=80):
    import numpy as np
    import random
    random.seed(seed)
    def gen(func, limit):
        csum = 0
        while True:
            val = func()
            csum += val
            if csum < limit:
                yield val
            else:
                yield val - csum + limit
                break
    a0 = b''.join(b * times for times, b in zip(gen(lambda : random.randrange(255), width * height), itertools.cycle([b'\x00', b'\xff'])))
    assert len(a0) == width * height
    return np.frombuffer(a0, dtype=np.uint8).reshape((height, width))

def frame_encoding_test(seed=None):
    a = _random_pixmap()
    frame = J2A.Frame(shape = a.shape[::-1], pixmap = a)
    frame.encode_image()

def mask_autogen_test(times = 1):
    from random import randint
    def bit_iter(a):
        for elt in a:
            for _ in range(8):
                yield elt & 1
                elt >>= 1
    try:
        for _ in range(times):
            width, height = randint(1, 50), randint(1, 50)
            pixmap = _random_pixmap(None, width, height)
#             pixmap = [[max(0, randint(-128, 127)) for _ in range(width)] for _ in range(height)]
            frame = J2A.Frame(shape = (width, height), pixmap = pixmap)
            frame.autogenerate_mask()
            mask = bytearray(frame.mask)
            for bit, pix in zip(bit_iter(mask), itertools.chain(*pixmap)):
                assert bit == bool(pix)
            assert mask[-1] >> (1 + ((width*height - 1) & 7)) == 0
    except AssertionError as e:
        print(width, height)
        print(pixmap)
        print(mask.hex())
        raise

def sample_serializing_test():
    anims = _read_hdr()
    for set_num, s in enumerate(anims.sets):
        samplesdata = bytearray(zlib.decompress(s._chunks[3][0], zlib.MAX_WBITS, s._chunks[3][1]))
        hdr_size = J2A.Sample._Header.size
        for sample_num, sample in enumerate(s.samples):
            newsampledata = sample.serialize()
            re_sample, _ = J2A.Sample.read(newsampledata, 0)
            for sprop in J2A.Sample.__slots__:
                assert getattr(re_sample, sprop) == getattr(sample, sprop), "Matching failed on set %d, frame %d" % (set_num, sample_num)
            # The "reserved2" field is dropped, even though it is not always zero in retail Anims.j2a:
            samplesdata[hdr_size-4:hdr_size] = b'\x00\x00\x00\x00'
            try:
                assert samplesdata.startswith(newsampledata), "Matching failed on set %d, frame %d" % (set_num, sample_num)
            except:
                for offset in range(0, len(newsampledata), 0x40):
                    print(  samplesdata[offset:offset+0x40].hex())
                    print(newsampledata[offset:offset+0x40].hex(), end='\n\n')
                raise
            samplesdata = samplesdata[len(newsampledata):]
    print("Test SUCCESSFUL")

def _encoding_strip(frame):
    def get_real_length(enc, height):
        assert isinstance(enc, bytearray)
        i = count = 0
        while i < len(enc):
            byte = enc[i]
            i += 1
            if byte > 0x80:
                i += byte - 0x80
            elif byte == 0x80:
                count += 1
                if count >= height:
                    return i
    enc = frame._rle_encoded_pixmap
    return enc[:get_real_length(enc, frame.shape[1])]

def gen_bigimage(filename):
    import more_itertools
    from PIL import Image

    anims = _read_hdr()
    global_width = 0
    main_map = []
    for set_num, s in enumerate(anims.sets):
        set_width = set_height = 0
        frames_l = []
        for anim_num, anim in enumerate(s.animations):
            frame = more_itertools.first_true(anim.frames, None, lambda frame : frame.shape > (1, 1))
            set_width += frame.shape[0]
            set_height = max(set_height, frame.shape[1])
            frames_l.append(frame.decode_image())
        main_map.append([frames_l, set_height])
        global_width = max(global_width, set_width)
    pixmap = []
    for frames_l, set_height in main_map:
        pixmap_add = [[] for _ in range(set_height)]
        for frame in frames_l:
            width, height = frame.shape
            for pix_row, frame_row in zip(pixmap_add, frame._pixmap):
                pix_row += frame_row
            for pix_row in pixmap_add[height:]:
                pix_row += [0] * width
        width = global_width - sum(frame.shape[0] for frame in frames_l)
        for pix_row in pixmap_add:
            pix_row += [0] * width
        pixmap += pixmap_add
    global_height = sum(set_height for _, set_height in main_map)
    assert all(len(row) == global_width for row in pixmap)
    assert len(pixmap) == global_height
    big_frame = J2A.Frame(shape = (global_width, global_height), pixmap = pixmap)
    image = anims.render_pixelmap(big_frame)
    image.save(filename)

def profile_func(funcname, mode, *pargs):
    '''
    Call function repeatedly according to the condition specified by `mode`.
    `funcname` specifies the name of the function in the global namespace to call.
    `mode` must be a string of one of the following types:
     - "#x", where # is an integer; this calls the function # times
     - "#s", where # is an integer; this calls the function for at least # seconds
    The function is called with `*pargs` positional arguments.
    This function is useful for profiling from the command line with a command such as:
    > python -m cProfile run_test.py profile_func <funcname> <mode> <args...>
    Optionally you can add `-o <file>` after "cProfile" to save the results to a file.
    Afterwards, to view it use:
    > python -m pstats <file>
    '''
    from time import time
    global fmap
    f = fmap[funcname]
    startingtime = time()
    if mode[-1] == "x":
        mode = int(mode[:-1])
        condition = lambda i,t : i < mode
    elif mode[-1] == "s":
        mode = float(mode[:-1])
        condition = lambda i,t : t <= mode
    else:
        raise KeyError

    curtime = startingtime
    for i in itertools.count():
        if condition(i, curtime-startingtime):
            f(*pargs)
            curtime = time()
        else:
            print("Running for {0:.3f} s, {1} iterations".format(curtime-startingtime, i))
            return

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
