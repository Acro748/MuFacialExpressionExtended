#include "Store.h"

namespace Mus {
	bool PerformanceCheck = false;
	bool PerformanceCheckAverage = true;

	std::atomic<bool> IsShowracemenuLoaded = false;
	std::atomic<bool> IsGamePaused = false;
}