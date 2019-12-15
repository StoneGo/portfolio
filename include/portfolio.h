//
// Created by stone on 11/12/19.
//
#ifndef COMMON_PORTFOLIO_H
#define COMMON_PORTFOLIO_H

#include "portfolio_global.h"

// C libraries
#include <cstdint>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cstdlib>


// C++ libraries
#include <string>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <stack>
#include <deque>

#define SESSION_ID_LENGTH 32
#define INSTRUMENT_LENGTH 32
#define PEOPLE_NAME_LENGTH 32
#define INTERFACE_NAME_LENGTH 32
#define ORDER_ID_LENGTH 32
#define SOFTWARE_NAME_LENGTH 32
#define STRATEGY_NAME_LENGTH 32

#define IP4_LENGTH 16
#define MAC_LENGTH 18
#define IP6_LENGTH 46


namespace portfolio {
using RAW_ORDER = void*;
using RAW_FULFILL = void*;
using Session_Instance = void*;
using OPERATOR_ID_TYPE = unsigned int;
using NUMERIC_ID = uint64_t;

enum INTERFACE_ID {
  INTERFACE_ID_NONE = 0,
  INTERFACE_ID_CF_CTP = 1,
};

enum EXCHANGE_ID {
  EXCHANGE_ID_NONE = 0,
  EXCHANGE_ID_SHFE = 1,
};

enum DIRECTION {
  DIRECTION_NONE = 0,
  DIRECTION_LONG = 1,
  DIRECTION_SHORT = -1,
};

enum ACTION {
  ACTION_NONE = 0,
  ACTION_OPEN = 1,
  ACTION_CLOSE = 2,
};

enum ORDER_TYPE {
  ORDER_TYPE_NONE = 0,
  ORDER_TYPE_LIMIT = 1,
  ORDER_TYPE_MARKET = 2,
};

enum ORDER_STATUS {
  ORDER_STATUS_NONE = 0,
  ORDER_STATUS_ACCEPTED = 1,
  ORDER_STATUS_REJECTED = 2,
  ORDER_STATUS_PART_TRADE = 3,
  ORDER_STATUS_ALL_TRADE = 4,
  ORDER_STATUS_RECALLING = 5,
  ORDER_STATUS_RECALLED = 6,
};

enum SESSION_STATUS {
  SESSION_STATUS_NONE = 0,
  SESSION_STATUS_NORMAL = 1,
};

class PORTFOLIO_API Interface {
public:
  Interface(INTERFACE_ID id, std::string nm) : ifid(id) {
    snprintf(name, INTERFACE_NAME_LENGTH, nm.c_str());
  }

  INTERFACE_ID ifid = INTERFACE_ID_NONE;
  char name[INTERFACE_NAME_LENGTH];
};

class PORTFOLIO_API Operator {
public:
  Operator(OPERATOR_ID_TYPE id, uint8_t prt, std::string nm
  ) : oid(id), priority(prt) {
    snprintf(name, PEOPLE_NAME_LENGTH, nm.c_str());
  }
  OPERATOR_ID_TYPE oid;
  uint8_t priority;
  char name[PEOPLE_NAME_LENGTH];
};

class Software {
public:
  NUMERIC_ID id = 0;
  char name[SOFTWARE_NAME_LENGTH];
};

class Strategy {
public:
  Strategy() {}
public:
  NUMERIC_ID id = 0;
  char name[STRATEGY_NAME_LENGTH];
};

class Session {
public:
  Session() {memset(this, 0, sizeof(Session));}

public:
  NUMERIC_ID sid;
  time_t start_time = 0;
  time_t end_time = 0;
  time_t last_act = 0;
  SESSION_STATUS status = SESSION_STATUS_NONE;
  Interface* market = nullptr;
  Session_Instance instance = nullptr;
  Operator* op = nullptr;
  Software* software = nullptr;
  int ip_port = 0;
  char raw_sid[SESSION_ID_LENGTH];
  char ip4[IP4_LENGTH];
  char mac4[MAC_LENGTH];
  char ip6[IP6_LENGTH];


public:
  void set_network_params(int fd) { UNUSED(fd); }
};

class Order {
public:
  Order() {}
  ~Order() {}

  DIRECTION dir = DIRECTION_NONE;
  ACTION action = ACTION_NONE;
  int volume = 0;
  double price = 0;
  ORDER_TYPE orderType = ORDER_TYPE_NONE;
  time_t start_time = 0;
  time_t accept_time = 0;
  ORDER_STATUS orderStatus = ORDER_STATUS_NONE;
  RAW_ORDER raw_order = nullptr;
  Session* session = nullptr;
  Strategy* strategy = nullptr;

  char order_id[ORDER_ID_LENGTH];
  char instrument[INSTRUMENT_LENGTH];

  void set_raw_order(RAW_ORDER ro) { raw_order = ro;}
};

class Fulfilled {
public:
  Fulfilled() {}
  ~Fulfilled() {}

  int volume = 0;
  time_t time = 0;
  char instrument[INSTRUMENT_LENGTH];
  Order* order = nullptr;
  RAW_FULFILL raw_fulfill = nullptr;
};

extern Interface gInterfaces[1];
extern Operator gOperator[2];
extern Software gSoftware[1];

using String2Interface = std::map<std::string, Interface*>;
extern String2Interface gIfMap;
using String2Software = std::map<std::string, Software*>;
extern String2Software gSoftwareMap;
using String2Operator = std::map<std::string, Operator*>;
extern String2Operator gOperatorMap;


using String2SessionMap = std::map<std::string, Session*>;
using Int2SessionMap = std::map<uint64_t, Session*>;

extern Int2SessionMap gOnlineSession;
extern String2SessionMap gRawSession;

extern Session* terminal_online(const char* raw_sid, const char* opname,
                                const char* software, const char* interface,
                                Session_Instance si, int fd);
extern void terminal_offline(Session* session);

extern void init_portfolio();
extern void release_portfolio();

} // namespace portfolio

#endif // COMMON_PORTFOLIO_H
