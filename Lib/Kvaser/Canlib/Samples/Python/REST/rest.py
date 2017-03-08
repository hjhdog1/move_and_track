import httplib
import socket
import json
import time
import logging
import inspect
import sys


#----------------------------------------------------------------------#
# RestAPI class                                                        #
#----------------------------------------------------------------------#

class RestAPI():

    def __init__(self, host, debug=None):
        self.host = host
        self.session_id = None

        fmt = '[%(levelname)s] %(funcName)s: %(message)s'
        if debug:
            logging.basicConfig(stream=sys.stderr,
                                level=logging.DEBUG,
                                format=fmt)
        else:
            logging.basicConfig(stream=sys.stderr,
                                level=logging.ERROR,
                                format=fmt)


    #------------------------------------------------------------------#
    # Status codes                                                     #
    #------------------------------------------------------------------#

    canOK                   =    0
    canERR_PARAM            =   -1
    canERR_NOMSG            =   -2
    canERR_NOCHANNELS       =   -5
    canERR_TIMEOUT          =   -7
    canERR_INVHANDLE        =  -10
    canERR_NOT_IMPLEMENTED  =  -32
    canERR_INVALID_PASSWORD = -128
    canERR_NO_SUCH_FUNCTION = -129
    canERR_NOT_AUTHORIZED   = -130
    canERR_INVALID_SESSION  = -131

    canOPEN_EXCLUSIVE           = 0x0008
    canOPEN_REQUIRE_EXTENDED    = 0x0010
    canOPEN_ACCEPT_VIRTUAL      = 0x0020
    canOPEN_OVERRIDE_EXCLUSIVE  = 0x0040
    canOPEN_REQUIRE_INIT_ACCESS = 0x0080
    canOPEN_NO_INIT_ACCESS      = 0x0100
    canOPEN_ACCEPT_LARGE_DLC    = 0x0200

    canBITRATE_1M   = -1
    canBITRATE_500K = -2
    canBITRATE_250K = -3
    canBITRATE_125K = -4
    canBITRATE_100K = -5
    canBITRATE_62K  = -6
    canBITRATE_50K  = -7
    canBITRATE_83K  = -8
    canBITRATE_10K  = -9

    canDRIVER_NORMAL        = 4
    canDRIVER_SILENT        = 1
    canDRIVER_SELFRECEPTION = 8
    canDRIVER_OFF           = 0

    canIOCTL_SET_TXACK       =  7
    canIOCTL_FLUSH_RX_BUFFER = 10

    usage_FREE      = 0
    usage_SERVICE   = 1
    usage_JSON_API  = 2
    usage_JSONP_API = 4


    #------------------------------------------------------------------#
    # Helper functions                                                 #
    #------------------------------------------------------------------#

    def clear_session_id(self):
        self.session_id = None

    def set_session_id(self, session_id):
        self.session_id = session_id

    def get_session_id(self):
        return self.session_id

    def get_data(self, uri):
        conn = httplib.HTTPConnection(self.host, timeout=3)
        try:
            conn.request("GET", uri)
            response = conn.getresponse()
            if response.status == 200:
                return response.read()
            else:
                print "get_data: bad status: %d" % response.status
                return None
        except (httplib.HTTPException, socket.error) as ex:
            print "get_data: exception: %s" % ex.args
            return None
        finally:
            conn.close()        

    def build_session_uri(self, cmd, params=None):
        if not self.session_id:
            logging.error("missing session id")
            return ""
        if params:
            return "/%s/%s?%s" % (self.session_id, cmd,
                                  '&'.join("%s=%s" % (key,val)
                                           for (key,val) in sorted(params.iteritems())))
        else:
            return "/%s/%s" % (self.session_id, cmd)

    def build_uri(self, cmd, params=None):
        if params:
            return "/%s?%s" % (cmd,
                               '&'.join("%s=%s" % (key,val)
                                        for (key,val) in sorted(params.iteritems())))
        else:
            return "/%s" % (cmd)

    def rest_req(self, cmd, params=None):
        uri = self.build_uri(cmd, params)
        logging.debug("uri="+uri)
        # print "uri="+uri

        enc_data = self.get_data(uri)
        if enc_data:
            # print enc_data
            return json.loads(enc_data)
        else:
            return None

    def rest_req_session(self, cmd, params=None):
        uri = self.build_session_uri(cmd, params)
        logging.debug("uri="+uri)
        # print "uri="+uri

        enc_data = self.get_data(uri)
        if enc_data:
            # print enc_data
            return json.loads(enc_data)
        else:
            return None

    #------------------------------------------------------------------#
    # REST calls (name these functions as the REST function name)      #
    #    - inspect.stack()[0][3] is the current functions name         #
    #------------------------------------------------------------------#

    def deviceStatus(self, params=None):
        return self.rest_req(inspect.stack()[0][3], params)

    def canInitializeLibrary(self, params=None):
        return self.rest_req(inspect.stack()[0][3], params)

    def canUnloadLibrary(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canOpenChannel(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canClose(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canSetBusParams(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canBusOn(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canBusOff(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canSetBusOutputControl(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canRead(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canWrite(self, params=None):
        if 'msg' in params:
            params['msg'] = ','.join(map(str, params['msg']))
        # if params['dlc'] == 0:
        #     params['msg'] = 0
        return self.rest_req_session(inspect.stack()[0][3], params)

    def canIoCtl(self, params=None):
        return self.rest_req_session(inspect.stack()[0][3], params)


if __name__ == '__main__':
    r = RestAPI("10.0.3.46:8080")
    print r.deviceStatus({"ident": 2221})
    data = r.canInitializeLibrary({"password": "mr.kvaser",
                                   "timeout": 5})
    print data
    r.set_session_id(data['session'])

    data = r.canOpenChannel({"channel": 0, "flags": RestAPI.canOPEN_ACCEPT_VIRTUAL})
    print data
    hnd = data['hnd']
    print r.canSetBusParams({"hnd": hnd, "freq": RestAPI.canBITRATE_1M})
    print r.canBusOn({"hnd": hnd})
    
    while True:
        try:
            msg = r.canRead({"hnd": hnd, "max": 50} )
            if msg['stat'] == RestAPI.canOK:
                print msg
            else:
                time.sleep(0.1)
        except (KeyboardInterrupt, SystemExit):
                break

    print r.canBusOff({"hnd": hnd})
    print r.canClose({"hnd": hnd})
    print r.canUnloadLibrary()
