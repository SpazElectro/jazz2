import re
import struct

class NamedStruct(struct.Struct):
    __slots__ = ["_names"]
    _fmtvalidate = re.compile(r"^((\d+[sp]|[xcbB?hHiIlLqQnNefd])\|\w+/)*(\d+[sp]|[xcbB?hHiIlLqQnNefd])\|\w+$")

    def __init__(self, fmt):
        assert NamedStruct._fmtvalidate.match(fmt)
        pairs = [it.split("|", 1) for it in fmt.split("/")]
        self._names = tuple(p[1] for p in pairs)
        super(NamedStruct, self).__init__("<" + "".join(p[0] for p in pairs))

    def pack(self, **kwargs):
        l = [None] * len(kwargs)
        for k,v in kwargs.items():
            l[self._names.index(k)] = v
        return super(NamedStruct, self).pack(*l)

    def pack_into(self, buffer, offset, **kwargs):
        l = [None] * len(kwargs)
        for k,v in kwargs.items():
            l[self._names.index(k)] = v
        return super(NamedStruct, self).pack_into(buffer, offset, *l)

    def unpack(self, *pargs, **kwargs):
        up = super(NamedStruct, self).unpack(*pargs, **kwargs)
        return dict(zip(self._names, up))

    def unpack_from(self, *pargs, **kwargs):
        up = super(NamedStruct, self).unpack_from(*pargs, **kwargs)
        return dict(zip(self._names, up))

    def iter_unpack(self, buffer, *pargs, **kwargs):
        offset = 0
        length = len(buffer)
        while offset < length:
            yield self.unpack_from(buffer, offset, *pargs, **kwargs)
            offset += self.size

    def iter_pack(self, iterable):
        return b''.join(self.pack(**d) for d in iterable)


def fake_crc(target_crc):
    target_crc ^= 0xffffffff
    for _ in range(32):
        target_crc <<= 1
        if target_crc & 0x100000000:
            target_crc ^= 0x1db710641
    return target_crc ^ 0xffffffff
