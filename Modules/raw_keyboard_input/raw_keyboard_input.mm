#ifdef __APPLE__
#include <TargetConditionals.h>
#ifdef TARGET_OS_IPHONE
#include "src/IOSKeyboard.mm"
#else
#include "src/MacOsKeyboard.mm"
#endif
#include "src/Keyboard.cpp"
#include "src/KeyboardFactory.cpp"
#endif
