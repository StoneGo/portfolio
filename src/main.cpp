#include <iostream>
#include "portfolio.h"

int main() {
  portfolio::init_portfolio();
  portfolio::Session* s = portfolio::terminal_online("123456", "stone go!", "Eureka BIT",
    "China Future CTP", nullptr, 0);
  std::cout << "Hello, " << s->sid << std::endl;
  portfolio::terminal_offline(s);
  portfolio::release_portfolio();
  return 0;
}
