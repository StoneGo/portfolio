//
// Created by stone on 11/12/19.
//

// #include <uv.h>

#include "portfolio.h"

typedef uint64_t uv_timeval64_t;

uv_timeval64_t tv;
void uv_gettimeofday(uv_timeval64_t* tv) {
    UNUSED(tv);
}

namespace portfolio {

Int2SessionMap gOnlineSession;
String2SessionMap gRawSession;
String2Interface gIfMap;
String2Software gSoftwareMap;
String2Operator gOperatorMap;


Interface gInterfaces[1] = {
  {INTERFACE_ID_NONE, "China Future CTP"}
};

Operator gOperator[2] = {
  {1, 1, "stone go!"},
  {2, 128, "superuser"},
};

Software gSoftware[1] = {
  {1, "Eureka BIT"}
};


Session* terminal_online(const char* raw_sid, const char* opname,
                         const char* software, const char* interface,
                         Session_Instance si, int fd) {
  if (gIfMap.find(interface) == gIfMap.end()
     || gOperatorMap.find(opname) == gOperatorMap.end()
     || gSoftwareMap.find(software) == gSoftwareMap.end())
    return 0;


  uv_timeval64_t tv;
  uv_gettimeofday(&tv);

  Session* s = new Session;
  s->instance = gIfMap[interface];
  s->software = gSoftwareMap[software];
  s->op = gOperatorMap[opname];
  s->instance = si;
  snprintf(s->raw_sid, SESSION_ID_LENGTH, raw_sid);
  // s->sid = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  // s->last_act = s->start_time = static_cast<time_t>(tv.tv_sec);
  s->end_time = 0;
  s->set_network_params(fd);

  //
  // set ip and port from fd
  //

  gOnlineSession.insert(std::make_pair(s->sid, s));
  gRawSession.insert(std::make_pair(raw_sid, s));

  return s;
}

void terminal_offline(Session* s) {
  if (s == nullptr) return;

  auto it = gOnlineSession.find(s->sid);
  if (it != gOnlineSession.end()) {
    auto it2 = gRawSession.find((it->second)->raw_sid);
    if (it2 != gRawSession.end())
      gRawSession.erase(it2);

    if (it->second) delete (it->second);
    gOnlineSession.erase(it);
  }
}

void init_portfolio() {
  gIfMap.insert(std::make_pair("China Future CTP", gInterfaces + 0));
  gSoftwareMap.insert(std::make_pair("Eureka BIT", gSoftware + 0));
  gOperatorMap.insert(std::make_pair("stone go!", gOperator + 0));
  gOperatorMap.insert(std::make_pair("superuser", gOperator + 1));
}

void release_portfolio() {
  for (auto& pair : gOnlineSession)
    if (pair.second != nullptr) delete pair.second;

  gOnlineSession.clear();
  gRawSession.clear();

  gOperatorMap.clear();
  gSoftwareMap.clear();
  gIfMap.clear();
}


} // namespace portfolio
