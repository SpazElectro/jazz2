import threading
import socket
import random
import time

from helpers import jj2
from helpers.functions import fetch_one, udpchecksum, preferred, unpreferred
from helpers.exceptions import ServerUnknownException

class pinger(threading.Thread):
    """
    Send message to connected ServerNet mirrors

    To be threaded, as multiple messages may need to be sent and messages may time out, etc
    """

    its_time = 0
    looping = True

    def __init__(self, ip=None, data=None, ls=None):
        """
        Set up sender

        Note that this does no checking of whether the address is a valid mirror; this is done in the main thread

        :param ip: IP address of mirror to send to
        :param data: Data to send
        :param ls: List server thread reference, for logging etc
        """
        threading.Thread.__init__(self)

        self.ip = ip
        self.data = data
        self.ls = ls

    def run(self):
        """
        Pings servers
        
        Servers can "cheat" by not telling the list server that they are client, but telling clients they are. This
        periodically sends a ping to the servers, similarly to clients, to get their actual status.
        
        This doesn't delist servers if that fails because it relies on UDP packets arriving in a timely manner and UDP
        is fundamentally unreliable, though failing may cause servers to be listed lower in the list.

        :return: Nothing
        """
        self.ls.log.info("Starting server pinger")

        while self.looping:
            time.sleep(10)
            current_time = int(time.time())

            server = fetch_one("SELECT id FROM servers WHERE origin = ? AND last_ping < ? ORDER BY last_ping ASC",
                               (self.ls.address, current_time - 300))
            if not server:
                self.ls.log.info("No servers to request ping of")
                continue

            try:
                jj2server = jj2.jj2server(server["id"], create_if_unknown=False)
            except ServerUnknownException:
                # this can happen if the server was delisted while it was being
                # loaded by the server pinger, in which case, no need to ping
                # it
                continue

            jj2server.set("last_ping", current_time)

            querysocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            querysocket.settimeout(5)
            try:
                address = (jj2server.get("ip"), int(jj2server.get("port")))
            except Exception:
                self.ls.log.warning("Server %s could not be initialised in server pinger!" % repr(jj2server))
                jj2server.forget()
                querysocket.close()
                continue

            index = random.choice(range(1, 7))
            dgram = udpchecksum(bytearray(
                [0x79, 0x79, 0x03, index, 0x00, 0x00, 0x00, 0x00, 0x32, 0x34, 0x20, 0x20]))
            #                 ^- ping command                     ^-----^- version

            old_prefer = jj2server.get("prefer")
            try:
                querysocket.sendto(dgram, address)
                data, srv = querysocket.recvfrom(1024)
                
                private = (data[8] >> 5) & 1

                if jj2server.get("private") != private:
                    jj2server.set("private", private)
                if preferred(jj2server.get("ip"), jj2server.get("name")):
                    jj2server.set("prefer", 2)
                elif unpreferred(jj2server.get("ip"), jj2server.get("name")):
                    jj2server.set("prefer", -1)
                else:
                    jj2server.set("prefer", 1)
                self.ls.log.info("Requested status packet from server %s" % jj2server.get("ip"))
            except(socket.timeout, TimeoutError, ConnectionError) as e:
                self.ls.log.warning("Server %s did not respond to status packet request (%s)" % (jj2server.get("ip"), e))
                jj2server.set("prefer", 0)  # don't delist, but make sure it's sorted to the bottom
                pass
            finally:
                try:
                    querysocket.shutdown(socket.SHUT_WR)
                    querysocket.close()
                except Exception as e:
                    pass

                new_prefer = jj2server.get("prefer")
                if new_prefer != old_prefer:
                    self.ls.broadcast(action="server", data=[jj2server.flush_updates()])


    def halt(self):
        self.looping = False
