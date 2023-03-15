#include <videoserver.hpp>

#include <iostream>

int main() {
  Videoserver server("127.0.0.1", 1337);
  server.httpSync();
}
