#include "emulator.hh"

int main(int argc, char **argv)
{
  Emulator *em = Emulator::get_instance();

  return em->run();
}
